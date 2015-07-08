#ifndef __DHCPCLIENT_H__
#define __DHCPCLIENT_H__

#include "../uip/uip.h"

#define DHCPCLIENT_IP_BROADCAST_OCTET   255
#define DHCPCLIENT_IP_SOURCE_PORT       68
#define DHCPCLIENT_IP_DESTINATION_PORT  67

/*
 * Check if DHCP resolving is finished.
 */
#define dhcpclient_is_done()    (dhcpclient_state == DHCPCLIENT_STATE_FINISHED)

enum dhcpclient_state {
    DHCPCLIENT_STATE_INIT,
    DHCPCLIENT_STATE_INITIALIZED,
    DHCPCLIENT_STATE_DISCOVER_PENDING,
    DHCPCLIENT_STATE_DISCOVER_SENT,
    DHCPCLIENT_STATE_OFFER_RECEIVED,
    DHCPCLIENT_STATE_REQUEST_PENDING,
    DHCPCLIENT_STATE_REQUEST_SENT,
    DHCPCLIENT_STATE_ACK_RECEIVED,
    DHCPCLIENT_STATE_ADDRESS_CONFIGURED,
    DHCPCLIENT_STATE_FINISHED
};

struct dhcpclient_session {
    uint8_t *buffer;
    uint16_t length;
    // TODO: remove duplicit code (perhaps create own data type for xid)
    uint8_t xid[4];
    uip_ipaddr_t client_address;
    uip_ipaddr_t server_address;
    uip_ipaddr_t netmask;
    uip_ipaddr_t dns;
    dhcp_lease_time_t lease_time;
};

extern enum dhcpclient_state dhcpclient_state;

void dhcpclient_init(void);
void dhcpclient_process(void);

void dhcpclient_appcall(void);
#endif
