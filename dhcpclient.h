#ifndef __DHCPCLIENT_H__
#define __DHCPCLIENT_H__

#include "uip/uip.h"

#define DHCPCLIENT_IP_BROADCAST_OCTET   255
#define DHCPCLIENT_IP_SOURCE_PORT       68
#define DHCPCLIENT_IP_DESTINATION_PORT  67

enum dhcpclient_state {
    DHCPCLIENT_STATE_INIT,
    DHCPCLIENT_STATE_DISCOVER_PENDING,
    DHCPCLIENT_STATE_DISCOVER_SENT,
    DHCPCLIENT_STATE_OFFER_RECEIVED,
    DHCPCLIENT_STATE_REQUEST_PENDING,
    DHCPCLIENT_STATE_REQUEST_SENT,
    DHCPCLIENT_STATE_ACK_RECEIVED
};

struct dhcpclient_session {
    uint8_t *buffer;
    uint16_t length;
    enum dhcpclient_state dhcpclient_state;
    uint32_t yiaddr;
    uint32_t siaddr;
    uint32_t netmask;
    uint16_t lease_time;
};

void dhcpclient_init(void);
void dhcpclient_process(void);

void dhcpclient_appcall(void);
#endif
