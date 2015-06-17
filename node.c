#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "uip/timer.h"
#include "uip/uip.h"
#include "dht.h"
#include "node.h"
#include "sharedbuf.h"
#include "dhcpclient.h"
#include "config.h"

#include "uart.h"

#define update_state(state)     (node_system_state = state)
#define current_state           node_system_state

/* Static function prototypes. */
static void handle_message(struct umqtt_connection __attribute__((unused)) *conn, char *topic, uint8_t *data, int len);
static void node_handle_connection_established(void);
static void node_handle_disconnected_wait(void);
static void node_send_data(void);
static void node_broker_connect(void);
static void node_mqtt_init(void);
static void node_umqtt_keep_alive(struct umqtt_connection *conn);


static uint8_t *send_buffer = sharedbuf + SHAREDBUF_NETHANDLER_OFFSET;
// TODO: make this variable uint16_t
static int16_t send_length;

/* Current system state */
enum node_system_state node_system_state;

/* MQTT connection structure instance. */
struct umqtt_connection mqtt = {
    .txbuff = {
        .start = sharedbuf + SHAREDBUF_NODE_UMQTT_TX_OFFSET,
        .length = SHAREDBUF_NODE_UMQTT_TX_SIZE,
    },
    .rxbuff = {
        .start = sharedbuf + SHAREDBUF_NODE_UMQTT_RX_OFFSET,
        .length = SHAREDBUF_NODE_UMQTT_RX_SIZE,
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
    
    dhcpclient_init();
    
    update_state(NODE_DHCP_QUERYING);
}

void node_process(void) {
    switch (current_state) {
        case NODE_DHCP_QUERYING:
            dhcpclient_process();
            break;
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
            node_umqtt_keep_alive(&mqtt);
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
        // TODO: remove hardcoded constant
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
    update_state(NODE_BROKER_CONNECTING);
}

static void node_mqtt_init(void) {
    umqtt_init(&mqtt);
    umqtt_circ_init(&mqtt.txbuff);
    umqtt_circ_init(&mqtt.rxbuff);
    umqtt_connect(&mqtt, MQTT_KEEP_ALIVE, MQTT_CLIENT_ID);
}

void node_notify_broker_unreachable(void) {
    timer_restart(&disconnected_wait_timer);
    update_state(NODE_BROKER_DISCONNECTED_WAIT);
}

static void node_umqtt_keep_alive(struct umqtt_connection *conn) {
    umqtt_ping(conn);
}

// TODO: for debug only
#define put_spacer()    uart_puts("  |  ")
__attribute__ ((unused)) static void print_uip_flags(void) {
    if(uip_flags) {
        uart_puts("  ");
        if(uip_acked())     uart_puts("acked"); else uart_puts("*");        put_spacer();
        if(uip_newdata())   uart_puts("newdata"); else uart_puts("*");      put_spacer();
        if(uip_rexmit())    uart_puts("rexmit"); else uart_puts("*");       put_spacer();
        if(uip_poll())      uart_puts("poll"); else uart_puts("*");         put_spacer();
        if(uip_closed())    uart_puts("closed"); else uart_puts("*");       put_spacer();
        if(uip_aborted())   uart_puts("aborted"); else uart_puts("*");      put_spacer();
        if(uip_connected()) uart_puts("connected"); else uart_puts("*");    put_spacer();
        if(uip_timedout())  uart_puts("timedout"); else uart_puts("*"); uart_println("");
    } else {
        uart_println("no flags");
    }
}

void node_appcall(void) {
    struct umqtt_connection *conn = uip_conn->appstate.conn;
    
    if (uip_connected()) {
        update_state(NODE_BROKER_CONNECTION_ESTABLISHED);
    }
    
    if(uip_aborted() || uip_timedout() || uip_closed()) {
        if (current_state == NODE_BROKER_CONNECTING) {
            /* Another disconnect in reconnecting phase. Shut down for a while, then try again. */
            node_notify_broker_unreachable();
        } else if (current_state != NODE_BROKER_DISCONNECTED_WAIT) {
            /* We are not waiting for atother reconnect try. */
            update_state(NODE_BROKER_DISCONNECTED);
            mqtt.state = UMQTT_STATE_INIT;
        }
    }
    
    if (uip_newdata()) {
        umqtt_circ_push(&conn->rxbuff, uip_appdata, uip_datalen());
        umqtt_process(conn);
    }
    
    if (uip_rexmit()) {
        uip_send(send_buffer, send_length);
    } else if (uip_poll() || uip_acked()) {
        send_length = umqtt_circ_pop(&conn->txbuff, send_buffer, sizeof(send_buffer));
        if (!send_length)
            return;
        uip_send(send_buffer, send_length);
    }
}

void node_udp_appcall(void) {
    switch (current_state) {
        case NODE_DHCP_QUERYING:
            dhcpclient_appcall();
            break;
        case NODE_DNS_QUERYING:
            break;
        default:
            break;
    }
}
