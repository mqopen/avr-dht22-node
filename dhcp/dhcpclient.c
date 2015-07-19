#include <stdlib.h>
#include <string.h>
#include "../uip/uip.h"
#include "../uip/timer.h"
#include "../sharedbuf.h"
#include "dhcp.h"
#include "dhcpclient.h"

#include "../uart.h"

#define update_state(state)     do {                                    \
                                    dhcpclient_state = state;           \
                                    timer_restart(&retry_timer);        \
                                } while (0)
#define current_state           dhcpclient_state
#define RETRY_TIMER_PERIOD      (CLOCK_SECOND * 5)

enum dhcpclient_state dhcpclient_state;

struct dhcpsession dhcpclient_data = {
// TODO: remove this directive
#if CONFIG_DHCP
    .buffer = sharedbuf.dhcp.buffer,
#endif
    .length = 0
};

/* Timer for sending retries. */
static struct timer retry_timer;

/* static function prototypes. */
static inline void _create_connection(void);
static inline void _on_retry_timer(void);
static inline void _handle_message(void);
static inline void _configure_address(void);

void dhcpclient_init(void) {
    update_state(DHCPCLIENT_STATE_INIT);
    timer_set(&retry_timer, RETRY_TIMER_PERIOD);
    /* Clear shared memory. */
    sharedbuf_clear();
    /* Generate xid. */
    dhcpclient_data.xid[0] = (uint8_t) rand();
    dhcpclient_data.xid[1] = (uint8_t) rand();
    dhcpclient_data.xid[2] = (uint8_t) rand();
    dhcpclient_data.xid[3] = (uint8_t) rand();
}

void dhcpclient_process(void) {
    if (timer_expired(&retry_timer))
        _on_retry_timer();
    switch (current_state) {
        case DHCPCLIENT_STATE_INIT:
            _create_connection();
            update_state(DHCPCLIENT_STATE_INITIALIZED);
        case DHCPCLIENT_STATE_INITIALIZED:
            dhcp_create_discover(&dhcpclient_data);
            update_state(DHCPCLIENT_STATE_DISCOVER_PENDING);
            break;
        case DHCPCLIENT_STATE_OFFER_RECEIVED:
            dhcp_create_request(&dhcpclient_data);
            update_state(DHCPCLIENT_STATE_REQUEST_PENDING);
            break;
        case DHCPCLIENT_STATE_ACK_RECEIVED:
            _configure_address();
            update_state(DHCPCLIENT_STATE_ADDRESS_CONFIGURED);
            break;
        default:
            break;
    };
}

void dhcpclient_appcall(void) {
    if (current_state == DHCPCLIENT_STATE_ADDRESS_CONFIGURED) {
        uip_close();
        update_state(DHCPCLIENT_STATE_FINISHED);
    }

    if (uip_newdata())
        _handle_message();

    if (uip_poll()) {
        switch (current_state) {
            case DHCPCLIENT_STATE_DISCOVER_PENDING:
                /* Simply transmitt dhcpclient_data stored in shared buffer. */
                uip_send(dhcpclient_data.buffer, dhcpclient_data.length);
                update_state(DHCPCLIENT_STATE_DISCOVER_SENT);
                break;
            case DHCPCLIENT_STATE_REQUEST_PENDING:
                uip_send(dhcpclient_data.buffer, dhcpclient_data.length);
                update_state(DHCPCLIENT_STATE_REQUEST_SENT);
                break;
            default:
                break;
        }
    }
}

static inline void _create_connection(void) {
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

static inline void _on_retry_timer(void) {
    switch (current_state) {
        case DHCPCLIENT_STATE_DISCOVER_SENT:
        case DHCPCLIENT_STATE_REQUEST_SENT:
            update_state(DHCPCLIENT_STATE_INITIALIZED);
            break;
        default:
            timer_restart(&retry_timer);
            break;
    }
}

static inline void _handle_message(void) {
    memcpy(dhcpclient_data.buffer, uip_appdata, uip_datalen());
    dhcpclient_data.length = uip_datalen();
    switch (current_state) {
        case DHCPCLIENT_STATE_DISCOVER_SENT:
            // TODO: handle multiple offers
            if (dhcp_process_offer(&dhcpclient_data))
                update_state(DHCPCLIENT_STATE_OFFER_RECEIVED);
            break;
        case DHCPCLIENT_STATE_REQUEST_SENT:
            if (dhcp_process_ack(&dhcpclient_data))
                update_state(DHCPCLIENT_STATE_ACK_RECEIVED);
            break;
        default:
            break;
    }
}

static inline void _configure_address(void) {
    uip_sethostaddr(&dhcpclient_data.client_address);
    uip_setnetmask(&dhcpclient_data.netmask);
}
