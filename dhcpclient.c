#include <string.h>
#include "uip/uip.h"
#include "sharedbuf.h"
#include "dhcp.h"
#include "dhcpclient.h"

#define update_state(state)     (data.dhcpclient_state = state)
#define current_state           data.dhcpclient_state

static struct dhcpclient_session data = {
    .buffer = sharedbuf + SHAREDBUF_DHCP_OFFSET,
    .length = 0
};

/* static function prototypes. */
static void _create_connection(void);

void dhcpclient_init(void) {
    update_state(DHCPCLIENT_STATE_INIT);
    
    /* Clear shared memory. */
    sharedbuf_clear();
}

void dhcpclient_process(void) {
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

void dhcpclient_handle_message(void) {
    switch (current_state) {
        case DHCPCLIENT_STATE_DISCOVER_SENT:
            update_state(DHCPCLIENT_STATE_OFFER_RECEIVED);
            break;
        case DHCPCLIENT_STATE_REQUEST_SENT:
            update_state(DHCPCLIENT_STATE_ACK_RECEIVED);
            break;
        default:
            break;
    }
}

void dhcpclient_appcall(void) {
    if (uip_newdata()) {
        memcpy(data.buffer, uip_appdata, uip_datalen());
        dhcpclient_handle_message();
    }

    if (uip_poll()) {
        switch (current_state) {
            case DHCPCLIENT_STATE_DISCOVER_PENDING:
                /* Simply transmitt data stored in shared buffer. */
                uip_send(data.buffer, data.length);
                update_state(DHCPCLIENT_STATE_DISCOVER_SENT);
            case DHCPCLIENT_STATE_REQUEST_PENDING:
                uip_send(data.buffer, data.length);
                update_state(DHCPCLIENT_STATE_REQUEST_SENT);
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
