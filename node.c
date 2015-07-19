#if CONFIG_DHCP
#include "common/sectimer.h"
#include "dhcp/dhcpclient.h"
#endif

#include "uip/uip.h"
#include "umqtt/mqttclient.h"
#include "node.h"

#include "uart.h"

#define update_state(state)     (node_system_state = state)
#define current_state           node_system_state

/* Static function prototypes. */
#if CONFIG_DHCP
static void _node_set_dhcp_lease_timer(void);
static void _node_test_dhcp_lease_timer(void);
#endif

/* Current system state */
enum node_system_state node_system_state;

#if CONFIG_DHCP
/* Timer for periodic re-leasing of IP address. */
static struct sectimer dhcp_lease_sectimer;
#endif

void node_init(void) {
#if CONFIG_DHCP
    dhcpclient_init();
#endif
    mqttclient_init();
#if CONFIG_DHCP
    update_state(NODE_DHCP_QUERYING);
#else
    update_state(NODE_MQTT);
#endif
}

void node_process(void) {
    switch (current_state) {
#if CONFIG_DHCP
        case NODE_DHCP_QUERYING:
            dhcpclient_process();
            if (dhcpclient_is_done()) {
                _node_set_dhcp_lease_timer();
                update_state(NODE_MQTT);
            }
            break;
#endif
        case NODE_MQTT:
            mqttclient_process();
            break;
        default:
            break;
    }
}

void node_appcall(void) {
#if CONFIG_DHCP
    _node_test_dhcp_lease_timer();
#endif
    if (current_state == NODE_MQTT)
        mqttclient_appcall();
}

void node_udp_appcall(void) {
#if CONFIG_DHCP
    _node_test_dhcp_lease_timer();
#endif
    switch (current_state) {
#if CONFIG_DHCP
        case NODE_DHCP_QUERYING:
            dhcpclient_appcall();
            break;
#endif
        case NODE_DNS_QUERYING:
            break;
        default:
            break;
    }
}

#if CONFIG_DHCP
static void _node_set_dhcp_lease_timer(void) {
    /* Debug only: re-lease IP address every 10 seconds. */
    sectimer_set(&dhcp_lease_sectimer, CLOCK_SECOND * 10);
}
#endif

#if CONFIG_DHCP
static void _node_test_dhcp_lease_timer(void) {
    /* Test DHCP lease timer. */
    switch (current_state) {
        case NODE_DHCP_QUERYING:
            break;
        default:
            if (sectimer_tryrestart(&dhcp_lease_sectimer)) {
                uip_close();
                dhcpclient_init();
                update_state(NODE_DHCP_QUERYING);
            }
            break;
    }
}
#endif

#if CONFIG_DEBUG
#define put_spacer()    uart_puts("  |  ")
__attribute__ ((unused)) static void print_uip_flags(void) {
    if (uip_flags) {
        uart_puts("  ");
        if (uip_acked())        uart_puts("acked"); else uart_puts("*");        put_spacer();
        if (uip_newdata())      uart_puts("newdata"); else uart_puts("*");      put_spacer();
        if (uip_rexmit())       uart_puts("rexmit"); else uart_puts("*");       put_spacer();
        if (uip_poll())         uart_puts("poll"); else uart_puts("*");         put_spacer();
        if (uip_closed())       uart_puts("closed"); else uart_puts("*");       put_spacer();
        if (uip_aborted())      uart_puts("aborted"); else uart_puts("*");      put_spacer();
        if (uip_connected())    uart_puts("connected"); else uart_puts("*");    put_spacer();
        if (uip_timedout())     uart_puts("timedout"); else uart_puts("*");     uart_println("");
    } else {
        uart_println("no flags");
    }
}
#endif
