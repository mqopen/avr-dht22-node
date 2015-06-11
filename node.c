#include "uip/timer.h"
#include "node.h"

static void handle_message(struct umqtt_connection __attribute__((unused)) *conn, char *topic, uint8_t *data, int len);

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

void node_init(void) {
    timer_set(&keep_alive_timer, CLOCK_SECOND * MQTT_KEEP_ALIVE / 2);
}

void node_process(void) {
    if (node_system_state == NODE_BROKER_CONNECTION_ESTABLISHED) {
        if (mqtt.state == UMQTT_STATE_CONNECTED) {
            if (timer_tryrestart(&keep_alive_timer))
                nethandler_umqtt_keep_alive(&mqtt);
        }
    }
}

static void handle_message(struct umqtt_connection __attribute__((unused)) *conn, char *topic, uint8_t *data, int len) {
}
