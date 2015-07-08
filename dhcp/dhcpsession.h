#ifndef __DHCPSESSION_H__
#define __DHCPSESSION_H__

#include "dhcpfields.h"
#include "../uip/uip.h"

struct dhcpsession {
    uint8_t *buffer;
    uint16_t length;
    // TODO: remove duplicit code (perhaps create own data type for xid)
    uint8_t xid[4];
    uip_ipaddr_t client_address;
    uip_ipaddr_t server_address;
    uip_ipaddr_t netmask;
    uip_ipaddr_t dns;
    struct dhcp_lease_time lease_time;
};

#endif
