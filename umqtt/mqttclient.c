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
#include "umqtt.h"
#include "mqttclient.h"

#define send_buffer_length      sizeof(sharedbuf.mqtt.send_buffer)
#define current_state           _mqttclient_state
#define update_state(state)     (_mqttclient_state = state)

/** Current MQTT client state. */
static enum mqttclient_state _mqttclient_state;

/** Timer for ending MQTT Keep Alive messages. */
static struct timer keep_alive_timer;

/** Timer for sending DHT measurements. */
static struct timer dht_timer;

/** Timer for limit reconnect attempts. */
static struct timer disconnected_wait_timer;

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

/* Static function prototypes. */

/**
 * Process working MQTT client.
 */
static inline void _mqttclient_handle_connection_established();

static void _mqttclient_broker_connect(void);
static void _mqttclient_handle_disconnected_wait(void);
static void _mqttclient_send_data(void);
static void _mqttclient_mqtt_init(void);
static void _mqttclient_umqtt_keep_alive(struct umqtt_connection *conn);
static void _mqttclient_handle_message(struct umqtt_connection *conn, char *topic, uint8_t *data, int len);

/**
 * Send data over network.
 */
static inline void _mqttclient_send(void);

/** MQTT connection structure instance. */
struct umqtt_connection mqtt = {
    .txbuff = {
        .start = sharedbuf.mqtt.mqtt_tx,
        .length = SHAREDBUF_NODE_UMQTT_TX_SIZE,
    },
    .rxbuff = {
        .start = sharedbuf.mqtt.mqtt_rx,
        .length = SHAREDBUF_NODE_UMQTT_RX_SIZE,
    },
    .message_callback = _mqttclient_handle_message,
    .state = UMQTT_STATE_INIT
};

void mqttclient_init(void) {
    timer_set(&keep_alive_timer, CLOCK_SECOND * MQTT_KEEP_ALIVE / 2);
    timer_set(&dht_timer, CLOCK_SECOND * MQTT_PUBLISH_PERIOD);
    timer_set(&disconnected_wait_timer, CLOCK_SECOND);
    actsig_init(&_broker_signal, PD6, &DDRD, &PORTD, 100);
    update_state(MQTTCLIENT_BROKER_DISCONNECTED);
}

void mqttclient_process(void) {
    actsig_process(&_broker_signal);
    switch (current_state) {
        case MQTTCLIENT_BROKER_CONNECTION_ESTABLISHED:
            _mqttclient_handle_connection_established();
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
    struct umqtt_connection *conn = uip_conn->appstate.conn;

    if (uip_connected()) {
        update_state(MQTTCLIENT_BROKER_CONNECTION_ESTABLISHED);
        return;
    }

    if (uip_aborted() || uip_timedout() || uip_closed()) {
        if (current_state == MQTTCLIENT_BROKER_CONNECTING) {
            /* Another disconnect in reconnecting phase. Shut down for a while, then try again. */
            timer_restart(&disconnected_wait_timer);
            update_state(MQTTCLIENT_BROKER_DISCONNECTED);
        } else if (current_state != MQTTCLIENT_BROKER_DISCONNECTED_WAIT) {
            /* We are not waiting for atother reconnect try. */
            update_state(MQTTCLIENT_BROKER_DISCONNECTED);
            mqtt.state = UMQTT_STATE_INIT;
        }
        return;
    }

    if (uip_newdata()) {
        enum umqtt_client_state previous_state = mqtt.state;
        umqtt_circ_push(&conn->rxbuff, uip_appdata, uip_datalen());
        umqtt_process(conn);

        /* Check for connection event. */
        if (previous_state != UMQTT_STATE_CONNECTED && mqtt.state == UMQTT_STATE_CONNECTED) {

            /* Send presence message. */
            umqtt_publish(&mqtt,
                            MQTT_NODE_PRESENCE_TOPIC,
                            (uint8_t *) MQTT_NODE_PRESENCE_MSG_ONLINE,
                            sizeof(MQTT_NODE_PRESENCE_MSG_ONLINE),
                            _BV(UMQTT_OPT_RETAIN));
        }
        return;
    }

    if (uip_acked()) {
        _is_sending = false;
        return;
    }

    if (uip_rexmit()) {
        _mqttclient_send();
        return;
    }

    if (uip_poll()) {
        _mqttclient_send_length = umqtt_circ_pop(&conn->txbuff, _mqttclient_send_buffer, send_buffer_length);
        if (_mqttclient_send_length) {
            _is_sending = true;
            _mqttclient_send();
        }
        return;
    }
}

static inline void _mqttclient_handle_connection_established(void) {
    if (mqtt.state == UMQTT_STATE_INIT) {
        _mqttclient_mqtt_init();
    }
    if (!_is_sending) {
        if (mqtt.state == UMQTT_STATE_CONNECTED) {
            if (timer_tryrestart(&keep_alive_timer)) {
                _mqttclient_umqtt_keep_alive(&mqtt);
                return;
            }
            if (timer_tryrestart(&dht_timer)) {
                _mqttclient_send_data();
                return;
            }
        }
    }
}

static void _mqttclient_broker_connect(void) {
    struct uip_conn *uc;
    uip_ipaddr_t ip;

    uip_ipaddr(&ip, MQTT_BROKER_IP_ADDR0, MQTT_BROKER_IP_ADDR1, MQTT_BROKER_IP_ADDR2, MQTT_BROKER_IP_ADDR3);
    uc = uip_connect(&ip, htons(MQTT_BROKER_PORT));
    if (uc == NULL) {
        return;
    }
    uc->appstate.conn = &mqtt;
    update_state(MQTTCLIENT_BROKER_CONNECTING);
}

static void _mqttclient_handle_disconnected_wait(void) {
    if (timer_tryrestart(&disconnected_wait_timer))
        _mqttclient_broker_connect();
}

static void _mqttclient_send_data(void) {
    enum dht_read_status status = dht_read();
    // TODO: remove hardcoded constant
    char buffer[20];
    uint8_t len = 0;
    switch (status) {
        case DHT_OK:
            /* If status is OK, publish measured data and return from function. */
            len = snprintf(buffer, sizeof(buffer), "%d.%d", dht_data.humidity / 10, dht_data.humidity % 10);
            umqtt_publish(&mqtt, MQTT_TOPIC_HUMIDITY, (uint8_t *)buffer, len, 0);
            len = snprintf(buffer, sizeof(buffer), "%d.%d", dht_data.temperature / 10, dht_data.temperature % 10);
            umqtt_publish(&mqtt, MQTT_TOPIC_TEMPERATURE, (uint8_t *)buffer, len, 0);
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
    umqtt_publish(&mqtt, MQTT_TOPIC_HUMIDITY, (uint8_t *)buffer, len, 0);
    umqtt_publish(&mqtt, MQTT_TOPIC_TEMPERATURE, (uint8_t *)buffer, len, 0);
}

static void _mqttclient_mqtt_init(void) {
    umqtt_init(&mqtt);
    umqtt_circ_init(&mqtt.txbuff);
    umqtt_circ_init(&mqtt.rxbuff);
    umqtt_connect(&mqtt, &_connection_config);
}

static void _mqttclient_umqtt_keep_alive(struct umqtt_connection *conn) {
    umqtt_ping(conn);
}

static void _mqttclient_handle_message(struct umqtt_connection *conn, char *topic, uint8_t *data, int len) {
}

static inline void _mqttclient_send(void) {
    actsig_notify(&_broker_signal);
    uip_send(_mqttclient_send_buffer, _mqttclient_send_length);
}
