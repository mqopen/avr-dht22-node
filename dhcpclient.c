#include <string.h>
#include "uip/uip.h"
#include "uip/timer.h"
#include "sharedbuf.h"
#include "dhcp.h"
#include "dhcpclient.h"

#include "uart.h"

#define update_state(state)     do {                                    \
                                    data.dhcpclient_state = state;      \
                                    timer_restart(&retry_timer);        \
                                } while (0)
#define current_state           data.dhcpclient_state
#define RETRY_TIMER_PERIOD      (CLOCK_SECOND * 1)

static struct dhcpclient_session data = {
    .buffer = sharedbuf + SHAREDBUF_DHCP_OFFSET,
    .length = 0
};

/* Timer for sending retries. */
static struct timer retry_timer;

/* static function prototypes. */
static void _create_connection(void);
static void _on_retry_timer(void);
static void _handle_message(void);

void dhcpclient_init(void) {
    update_state(DHCPCLIENT_STATE_INIT);
    timer_set(&retry_timer, RETRY_TIMER_PERIOD);
    /* Clear shared memory. */
    sharedbuf_clear();
}

void dhcpclient_process(void) {
    if (timer_expired(&retry_timer))
        _on_retry_timer();
    switch (current_state) {
        case DHCPCLIENT_STATE_INIT:
            _create_connection();
            dhcp_create_discover(&data);
            update_state(DHCPCLIENT_STATE_DISCOVER_PENDING);
            break;
        case DHCPCLIENT_STATE_OFFER_RECEIVED:
            dhcp_create_request(&data);
            update_state(DHCPCLIENT_STATE_REQUEST_PENDING);
            break;
        default:
            break;
    };
}

void dhcpclient_appcall(void) {
    if (uip_newdata())
        _handle_message();

    if (uip_poll()) {
        switch (current_state) {
            case DHCPCLIENT_STATE_DISCOVER_PENDING:
                /* Simply transmitt data stored in shared buffer. */
                uip_send(data.buffer, data.length);
                update_state(DHCPCLIENT_STATE_DISCOVER_SENT);
                break;
            case DHCPCLIENT_STATE_REQUEST_PENDING:
                uip_send(data.buffer, data.length);
                update_state(DHCPCLIENT_STATE_REQUEST_SENT);
                break;
            default:
                break;
        }
    }
}

static void _create_connection(void) {
    uip_ipaddr_t addr;
    struct uip_udp_conn *c;
    uip_ipaddr(&addr,
                DHCPCLIENT_IP_BROADCAST_OCTET,
                DHCPCLIENT_IP_BROADCAST_OCTET,
                DHCPCLIENT_IP_BROADCAST_OCTET,
                DHCPCLIENT_IP_BROADCAST_OCTET);
    c = uip_udp_new(&addr, HTONS(DHCPCLIENT_IP_DESTINATION_PORT));
    if (c != NULL) {
        uip_udp_bind(c, HTONS(DHCPCLIENT_IP_SOURCE_PORT));
    }
}

static void _on_retry_timer(void) {
    switch (current_state) {
        case DHCPCLIENT_STATE_DISCOVER_SENT:
            update_state(DHCPCLIENT_STATE_INIT);
            break;
        default:
            timer_restart(&retry_timer);
            break;
    }
}

static void _handle_message(void) {
    memcpy(data.buffer, uip_appdata, uip_datalen());
    switch (current_state) {
        case DHCPCLIENT_STATE_DISCOVER_SENT:
            dhcp_process_offer(&data);
            update_state(DHCPCLIENT_STATE_OFFER_RECEIVED);
            break;
        case DHCPCLIENT_STATE_REQUEST_SENT:
            dhcp_process_ack(&data);
            update_state(DHCPCLIENT_STATE_ACK_RECEIVED);
            break;
        default:
            break;
    }
}
