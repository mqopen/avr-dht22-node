#ifndef __DHCPCLIENT_H__
#define __DHCPCLIENT_H__

#include "uip/uip.h"

#define DHCPCLIENT_IP_BROADCAST_OCTET   255
#define DHCPCLIENT_IP_SOURCE_PORT       68
#define DHCPCLIENT_IP_DESTINATION_PORT  67

#define dhcpclient_is_done()    (dhcpclient_state == DHCPCLIENT_STATE_ADDRESS_CONFIGURED)

enum dhcpclient_state {
    DHCPCLIENT_STATE_INIT,
    DHCPCLIENT_STATE_DISCOVER_PENDING,
    DHCPCLIENT_STATE_DISCOVER_SENT,
    DHCPCLIENT_STATE_OFFER_RECEIVED,
    DHCPCLIENT_STATE_REQUEST_PENDING,
    DHCPCLIENT_STATE_REQUEST_SENT,
    DHCPCLIENT_STATE_ACK_RECEIVED,
    DHCPCLIENT_STATE_ADDRESS_CONFIGURED
};

struct dhcpclient_session {
    uint8_t *buffer;
    uint16_t length;
    uip_ipaddr_t yiaddr;
    uip_ipaddr_t siaddr;
    uip_ipaddr_t netmask;
    uint16_t lease_time;
};

extern enum dhcpclient_state dhcpclient_state;

void dhcpclient_init(void);
void dhcpclient_process(void);

void dhcpclient_appcall(void);
#endif
