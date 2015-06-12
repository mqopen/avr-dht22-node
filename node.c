#include <util/delay.h>
#include <stdio.h>
#include "uip/timer.h"
#include "uip/uip.h"
#include "dht.h"
#include "node.h"
#include "config.h"

/* Static function prototypes. */
static void handle_message(struct umqtt_connection __attribute__((unused)) *conn, char *topic, uint8_t *data, int len);
static void node_handle_connection_established(void);
static void node_handle_disconnected_wait(void);
static void node_send_data(void);
static void node_broker_connect(void);
static void node_mqtt_init(void);

enum node_system_state node_system_state;

/* MQTT RX and TX buffers. */
static uint8_t mqtt_txbuff[200];
static uint8_t mqtt_rxbuff[150];

/* MQTT connection structure instance. */
struct umqtt_connection mqtt = {
    .txbuff = {
        .start = mqtt_txbuff,
        .length = sizeof(mqtt_txbuff),
    },
    .rxbuff = {
        .start = mqtt_rxbuff,
        .length = sizeof(mqtt_rxbuff),
    },
    .message_callback = handle_message,
    .state = UMQTT_STATE_INIT
};

/* Timer for ending MQTT Keep Alive messages. */
static struct timer keep_alive_timer;

/* Timer for sending DHT measurements. */
static struct timer dht_timer;

/* Timer for limit reconnect attempts. */
static struct timer disconnected_wait_timer;

void node_init(void) {
    timer_set(&keep_alive_timer, CLOCK_SECOND * MQTT_KEEP_ALIVE / 2);
    timer_set(&dht_timer, CLOCK_SECOND * 2);
    timer_set(&disconnected_wait_timer, CLOCK_SECOND);
    node_system_state = NODE_BROKER_DISCONNECTED;
}

void node_process(void) {
    switch (node_system_state) {
        case NODE_BROKER_CONNECTION_ESTABLISHED:
            node_handle_connection_established();
            break;
        case NODE_BROKER_DISCONNECTED:
            node_broker_connect();
            break;
        case NODE_BROKER_DISCONNECTED_WAIT:
            node_handle_disconnected_wait();
            break;
        default:
            break;
    }
}

static void node_handle_connection_established(void) {
    if (mqtt.state == UMQTT_STATE_CONNECTED) {
        if (timer_tryrestart(&keep_alive_timer))
            nethandler_umqtt_keep_alive(&mqtt);
        if (timer_tryrestart(&dht_timer))
            node_send_data();
    } else if (mqtt.state == UMQTT_STATE_INIT) {
        node_mqtt_init();
    }
}

static void node_handle_disconnected_wait(void) {
    if (timer_tryrestart(&disconnected_wait_timer))
        node_broker_connect();
}

static void node_send_data(void) {
    enum dht_read_status status = dht_read();
    if(status == DHT_OK) {
        char buffer[20];
        uint8_t len;
        len = snprintf(buffer, sizeof(buffer), "%d.%d", dht_data.humidity / 10, dht_data.humidity % 10);
        umqtt_publish(&mqtt, MQTT_TOPIC_HUMIDITY, (uint8_t *)buffer, len);
        len = snprintf(buffer, sizeof(buffer), "%d.%d", dht_data.temperature / 10, dht_data.temperature % 10);
        umqtt_publish(&mqtt, MQTT_TOPIC_TEMPERATURE, (uint8_t *)buffer, len);
    }
}

static void handle_message(struct umqtt_connection __attribute__((unused)) *conn, char *topic, uint8_t *data, int len) {
}

static void node_broker_connect(void) {
    struct uip_conn *uc;
    uip_ipaddr_t ip;

    uip_ipaddr(&ip, MQTT_BROKER_IP_ADDR0, MQTT_BROKER_IP_ADDR1, MQTT_BROKER_IP_ADDR2, MQTT_BROKER_IP_ADDR3);
    uc = uip_connect(&ip, htons(MQTT_BROKER_PORT));
    if (uc == NULL) {
        return;
    }
    uc->appstate.conn = &mqtt;
    node_system_state = NODE_BROKER_CONNECTING;
}

static void node_mqtt_init(void) {
    umqtt_init(&mqtt);
    umqtt_circ_init(&mqtt.txbuff);
    umqtt_circ_init(&mqtt.rxbuff);
    umqtt_connect(&mqtt, MQTT_KEEP_ALIVE, MQTT_CLIENT_ID);
}

void node_notify_broker_unreachable(void) {
    timer_restart(&disconnected_wait_timer);
    node_system_state = NODE_BROKER_DISCONNECTED_WAIT;
}
