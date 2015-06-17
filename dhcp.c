#include <string.h>
#include "uip/uip.h"
#include "uip/uiparp.h"
#include "dhcp.h"

#define MSG(dhcp)                   ((struct dhcp_message *) dhcp->buffer)
#define LENGTH                      (dhcp->length)
#define add_to_end(dhcp, value)     (*(MSG(dhcp)->options + LENGTH++) = value)

static const uint8_t xid[] = {0xad, 0xde, 0x12, 0x23};
static const uint8_t magic_cookie[] = {99, 130, 83, 99};

/* Static finction prototypes. */
static void create_message(struct dhcpclient_data *dhcp);
static void add_message_type(struct dhcpclient_data *dhcp, uint8_t type);
static void add_server_id(struct dhcpclient_data *dhcp);
static void add_request_ip_address(struct dhcpclient_data *dhcp);
static void add_request_options(struct dhcpclient_data *dhcp);
static void add_end(struct dhcpclient_data *dhcp);

void dhcp_create_discover(struct dhcpclient_data *dhcp) {
    create_message(dhcp);
    add_message_type(dhcp, DHCP_MESSAGE_TYPE_DHCPDISCOVER);
    add_request_options(dhcp);
    add_end(dhcp);
}

void dhcp_create_request(struct dhcpclient_data *dhcp) {
    create_message(dhcp);
    add_message_type(dhcp, DHCP_MESSAGE_TYPE_DHCPREQUEST);
    add_server_id(dhcp);
    add_request_ip_address(dhcp);
}

static void create_message(struct dhcpclient_data *dhcp) {
    MSG(dhcp)->op = DHCP_OP_BOOTREQUEST;
    MSG(dhcp)->htype = DHCP_HTYPE_ETHERNET_10;
    MSG(dhcp)->hlen = 6;
    MSG(dhcp)->hops = 0;
    memcpy(&MSG(dhcp)->xid, xid, sizeof(MSG(dhcp)->xid));
    MSG(dhcp)->secs = 0;
    MSG(dhcp)->flags = HTONS(DHCP_FLAGS_BROADCAST);
    memcpy(&MSG(dhcp)->ciaddr, uip_hostaddr, sizeof(MSG(dhcp)->ciaddr));
    /* Always zero per rfc, server def. */
    memset(&MSG(dhcp)->yiaddr, 0, sizeof(MSG(dhcp)->yiaddr));
    memset(&MSG(dhcp)->siaddr, 0, sizeof(MSG(dhcp)->siaddr));
    memset(&MSG(dhcp)->giaddr, 0, sizeof(MSG(dhcp)->giaddr));
    memcpy(&MSG(dhcp)->chaddr, &uip_ethaddr, sizeof(struct uip_eth_addr));
    memset(&MSG(dhcp)->chaddr[sizeof(struct uip_eth_addr)], 0, sizeof(MSG(dhcp)->chaddr) - sizeof(struct uip_eth_addr));
    memset(MSG(dhcp)->sname, 0, sizeof(MSG(dhcp)->sname));
    memset(MSG(dhcp)->file, 0, sizeof(MSG(dhcp)->file));
    memcpy(MSG(dhcp)->options, magic_cookie, sizeof(magic_cookie));
    
    LENGTH = sizeof(struct dhcp_message) - DHCP_MESSAGE_OPTIONS_SIZE;
    LENGTH += sizeof(magic_cookie);
}

static void add_message_type(struct dhcpclient_data *dhcp, uint8_t type) {
    add_to_end(dhcp, DHCP_OPTION_MSG_TYPE);
    add_to_end(dhcp, DHCP_OPTION_MSG_TYPE_LENGTH);
    add_to_end(dhcp, type);
}

static void add_server_id(struct dhcpclient_data *dhcp) {
    add_to_end(dhcp, DHCP_OPTION_SERVER_ID);
    add_to_end(dhcp, 4);
}

static void add_request_ip_address(struct dhcpclient_data *dhcp) {
    add_to_end(dhcp, DHCP_OPTION_REQ_IPADDR);
    add_to_end(dhcp, 4);
}

static void add_request_options(struct dhcpclient_data *dhcp) {
    add_to_end(dhcp, DHCP_OPTION_REQ_LIST);
    add_to_end(dhcp, 3);
    add_to_end(dhcp, DHCP_OPTION_SUBNET_MASK);
    add_to_end(dhcp, DHCP_OPTION_ROUTER);
    add_to_end(dhcp, DHCP_OPTION_DNS_SERVER);
}

static void add_end(struct dhcpclient_data *dhcp) {
    add_to_end(dhcp, DHCP_OPTION_END);
}
