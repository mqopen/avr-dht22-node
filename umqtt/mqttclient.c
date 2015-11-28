/*
 * Copyright (C) Ivo Slanina <ivo.slanina@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdbool.h>
#include "../config.h"
#include "../uip/uip.h"
#include "../uip/timer.h"
#include "../dht.h"
#include "../sharedbuf.h"
#include "../actsig.h"
#include "../config.h"
#include "umqtt.h"
#include "mqttclient.h"

#define current_state           _mqttclient_state
#define update_state(state)     (_mqttclient_state = state)

/** Current MQTT client state. */
static enum mqttclient_state _mqttclient_state;

/** Timer for ending MQTT Keep Alive messages. */
static struct timer _keep_alive_timer;

/** Timer for sending DHT measurements. */
static struct timer _dht_timer;

/** Timer for limit reconnect attempts. */
static struct timer _disconnected_wait_timer;

/** Send data buffer. */
static uint8_t *_mqttclient_send_buffer = sharedbuf.mqtt.send_buffer;

/** Send data length. */
static uint16_t _mqttclient_send_length;

/** Signaling network activity. */
static struct actsig_signal _broker_signal;

/** Keep track if some send is in progress. */
static bool _is_sending = false;

/** Connection configuration. */
static struct umqtt_connect_config _connection_config = {
    .keep_alive = MQTT_KEEP_ALIVE,
    .client_id = MQTT_CLIENT_ID,
    .will_topic = MQTT_NODE_PRESENCE_TOPIC,
    .will_message = (uint8_t *) MQTT_NODE_PRESENCE_MSG_OFFLINE,
    .will_message_len = sizeof(MQTT_NODE_PRESENCE_MSG_OFFLINE),
    .flags = _BV(UMQTT_OPT_RETAIN),
};

/**
 * Handle incomming message.
 *
 * @param conn MQTT connection.
 * @param topic Topic name.
 * @param data Topic payload.
 * @param len Payload length.
 */
static void _mqttclient_handle_message(struct umqtt_connection *conn, char *topic, uint8_t *data, uint16_t len);


/** MQTT connection structure instance. */
static struct umqtt_connection _mqtt = {
    .txbuff = {
        .start = sharedbuf.mqtt.mqtt_tx,
        .length = SHAREDBUF_NODE_UMQTT_TX_SIZE,
    },
    .rxbuff = {
        .start = sharedbuf.mqtt.mqtt_rx,
        .length = SHAREDBUF_NODE_UMQTT_RX_SIZE,
    },
    .message_callback = _mqttclient_handle_message,
    .state = UMQTT_STATE_INIT,
};

/* Static function prototypes. */

/**
 * Process working MQTT client.
 */
static inline void _mqttclient_process_connected();

/**
 * Create connection to MQTT broker.
 */
static void _mqttclient_broker_connect(void);

/**
 * Handle disconnect.
 */
static void _mqttclient_handle_disconnected_wait(void);

/**
 * Send data.
 */
static void _mqttclient_send_data(void);

/**
 * Initiate MQTT client.
 */
static void _mqttclient_mqtt_init(void);

/**
 * Send keep alive message.
 *
 * @param conn MQTT connection.
 */
static void _mqttclient_umqtt_keep_alive(struct umqtt_connection *conn);

/**
 * Handle new arrived data.
 */
static inline void _mqttclient_handle_new_data(void);

/**
 * Handle error condition.
 */
static inline void _mqttclient_handle_communication_error(void);

/**
 * Send data over network.
 */
static inline void _mqttclient_send(void);

/**
 * Pop uMQTT circ buffer and transfer it over network.
 */
static void _mqttclient_transfer_buffer(void);

/**
 * Signal established TCP connection with MQTT broker.
 */
static inline void _mqttclient_signal_connected(void);

/**
 * Signal lost TCP connection with MQTT broker.
 */
static inline void _mqttclient_signal_disconnected(void);

/* Implementation. */

void mqttclient_init(void) {
    _mqttclient_mqtt_init();
    timer_set(&_keep_alive_timer, CLOCK_SECOND * MQTT_KEEP_ALIVE / 2);
    timer_set(&_dht_timer, CLOCK_SECOND * MQTT_PUBLISH_PERIOD);
    timer_set(&_disconnected_wait_timer, CLOCK_SECOND);
    actsig_init(&_broker_signal,
                CONFIG_SIGNAL_LED_PIN,
                &CONFIG_SIGNAL_LED_DDR,
                &CONFIG_SIGNAL_LED_PORT,
                CONFIG_SIGNAL_LED_INTERVAL);
    update_state(MQTTCLIENT_BROKER_DISCONNECTED);
}

void mqttclient_process(void) {
    actsig_process(&_broker_signal);
    switch (current_state) {
        case MQTTCLIENT_BROKER_CONNECTION_ESTABLISHED:
            _mqttclient_process_connected();
            break;
        case MQTTCLIENT_BROKER_DISCONNECTED:
            _mqttclient_broker_connect();
            break;
        case MQTTCLIENT_BROKER_DISCONNECTED_WAIT:
            _mqttclient_handle_disconnected_wait();
            break;
        default:
            break;
    }
}

void mqttclient_appcall(void) {
    if (uip_poll()) {
        _mqttclient_transfer_buffer();
    } else if (uip_connected()) {
        update_state(MQTTCLIENT_BROKER_CONNECTION_ESTABLISHED);
        umqtt_connect(&_mqtt, &_connection_config);
        _mqttclient_transfer_buffer();
    } else if (uip_aborted() || uip_timedout() || uip_closed()) {
        _mqttclient_handle_communication_error();
    } else  if (uip_newdata()) {
        _mqttclient_handle_new_data();
    } else if (uip_acked()) {
        _is_sending = false;
    } else if (uip_rexmit()) {
        _mqttclient_send();
    }
}

static inline void _mqttclient_handle_new_data(void) {
    enum umqtt_client_state previous_state = _mqtt.state;
    umqtt_circ_push(&uip_conn->appstate.conn->rxbuff, uip_appdata, uip_datalen());
    umqtt_process(uip_conn->appstate.conn);

    /* Check for connection event. */
    if (previous_state != UMQTT_STATE_CONNECTED && _mqtt.state == UMQTT_STATE_CONNECTED) {

        /* Signal established connection. */
        _mqttclient_signal_connected();

        /* Send presence message. */
        umqtt_publish(&_mqtt,
                        MQTT_NODE_PRESENCE_TOPIC,
                        (uint8_t *) MQTT_NODE_PRESENCE_MSG_ONLINE,
                        sizeof(MQTT_NODE_PRESENCE_MSG_ONLINE),
                        _BV(UMQTT_OPT_RETAIN));
    }
}

static void _mqttclient_transfer_buffer(void) {
    _mqttclient_send_length = umqtt_circ_pop(&uip_conn->appstate.conn->txbuff,
                                                _mqttclient_send_buffer,
                                                sizeof(sharedbuf.mqtt.send_buffer));
    if (_mqttclient_send_length) {
        _is_sending = true;
        _mqttclient_send();
    }
}

static inline void _mqttclient_handle_communication_error(void) {
    _mqttclient_signal_disconnected();
    if (current_state == MQTTCLIENT_BROKER_CONNECTING) {
        /* Another disconnect in reconnecting phase. Shut down for a while, then try again. */
        timer_restart(&_disconnected_wait_timer);
        update_state(MQTTCLIENT_BROKER_DISCONNECTED);
    } else if (current_state != MQTTCLIENT_BROKER_DISCONNECTED_WAIT) {
        /* We are not waiting for atother reconnect try. */
        update_state(MQTTCLIENT_BROKER_DISCONNECTED);
        _mqtt.state = UMQTT_STATE_INIT;
    }
}

static inline void _mqttclient_process_connected(void) {
    if (!_is_sending) {
        if (_mqtt.state == UMQTT_STATE_CONNECTED) {
            if (timer_tryrestart(&_keep_alive_timer)) {
                _mqttclient_umqtt_keep_alive(&_mqtt);
                return;
            }
            if (timer_tryrestart(&_dht_timer)) {
                _mqttclient_send_data();
                return;
            }
        }
    }
}

static void _mqttclient_broker_connect(void) {
    struct uip_conn *uc;
    uip_ipaddr_t ip;

    uip_ipaddr(&ip,
                MQTT_BROKER_IP_ADDR0,
                MQTT_BROKER_IP_ADDR1,
                MQTT_BROKER_IP_ADDR2,
                MQTT_BROKER_IP_ADDR3);
    uc = uip_connect(&ip, htons(MQTT_BROKER_PORT));
    if (uc == NULL) {
        return;
    }
    uc->appstate.conn = &_mqtt;
    update_state(MQTTCLIENT_BROKER_CONNECTING);
}

static void _mqttclient_handle_disconnected_wait(void) {
    if (timer_tryrestart(&_disconnected_wait_timer))
        _mqttclient_broker_connect();
}

static void _mqttclient_send_data(void) {
    enum dht_read_status status = dht_read();
    // TODO: remove hardcoded constant
    char buffer[20];
    uint8_t len = 0;
    int16_t _val_integral;
    uint16_t _val_decimal;
    switch (status) {
        case DHT_OK:
            /* If status is OK, publish measured data and return from function. */
            _val_integral = dht_data.humidity / 10;
            _val_decimal = dht_data.humidity % 10;
            len = snprintf(buffer, sizeof(buffer), "%d.%u", _val_integral, _val_decimal);
            umqtt_publish(&_mqtt, MQTT_TOPIC_HUMIDITY, (uint8_t *)buffer, len, 0);
            if (dht_data.temperature < 0) {
                _val_integral = dht_data.temperature / 10;
                _val_decimal = -dht_data.temperature % 10;
            } else {
                _val_integral = dht_data.temperature / 10;
                _val_decimal = dht_data.temperature % 10;
            }
            len = snprintf(buffer, sizeof(buffer), "%d.%u", _val_integral, _val_decimal);
            umqtt_publish(&_mqtt, MQTT_TOPIC_TEMPERATURE, (uint8_t *)buffer, len, 0);
            return;
        case DHT_ERROR_CHECKSUM:
            len = snprintf(buffer, sizeof(buffer), "E_CHECKSUM");
            break;
        case DHT_ERROR_TIMEOUT:
            len = snprintf(buffer, sizeof(buffer), "E_TIMEOUT");
            break;
        case DHT_ERROR_CONNECT:
            len = snprintf(buffer, sizeof(buffer), "E_CONNECT");
            break;
        case DHT_ERROR_ACK:
            len = snprintf(buffer, sizeof(buffer), "E_ACK");
            break;
    }

    /* Publish error codes. */
    umqtt_publish(&_mqtt, MQTT_TOPIC_HUMIDITY, (uint8_t *)buffer, len, 0);
    umqtt_publish(&_mqtt, MQTT_TOPIC_TEMPERATURE, (uint8_t *)buffer, len, 0);
}

static void _mqttclient_mqtt_init(void) {
    umqtt_init(&_mqtt);
    umqtt_circ_init(&_mqtt.txbuff);
    umqtt_circ_init(&_mqtt.rxbuff);
}

static void _mqttclient_umqtt_keep_alive(struct umqtt_connection *conn) {
    umqtt_ping(conn);
}

static void _mqttclient_handle_message(struct umqtt_connection *conn, char *topic, uint8_t *data, uint16_t len) {
}

static inline void _mqttclient_send(void) {
    actsig_notify(&_broker_signal);
    uip_send(_mqttclient_send_buffer, _mqttclient_send_length);
}

static inline void _mqttclient_signal_connected(void) {
    actsig_set_normal_on(&_broker_signal);
}

static inline void _mqttclient_signal_disconnected(void) {
    actsig_set_normal_off(&_broker_signal);
}
