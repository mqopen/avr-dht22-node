#include <string.h>
#include "uip/uip.h"
#include "uip/uiparp.h"
#include "dhcp.h"

#define member_size(type, member)   sizeof(((type *)0)->member)
#define MSG(dhcp)                   ((struct dhcp_message *) dhcp->buffer)
#define LENGTH                      dhcp->length
#define OPTIONS_OFFSET              LENGTH - sizeof(struct dhcp_message) + member_size(struct dhcp_message, options)

static const uint8_t xid[] = {0xad, 0xde, 0x12, 0x23};
static const uint8_t magic_cookie[] = {0x63, 0x82, 0x53, 0x63};

/* Static function prototypes. */
static void _create_message(struct dhcpclient_session *dhcp);
static void _add_message_type(struct dhcpclient_session *dhcp, uint8_t type);
static void _add_server_id(struct dhcpclient_session *dhcp);
static void _add_request_ip_address(struct dhcpclient_session *dhcp);
static void _add_request_options(struct dhcpclient_session *dhcp);
static void _add_end(struct dhcpclient_session *dhcp);

static inline void _add_to_end_generic(struct dhcpclient_session *dhcp, void *value, uint8_t value_size) {
    memcpy(MSG(dhcp)->options + OPTIONS_OFFSET, value, value_size);
    LENGTH += value_size;
}

static inline void _add_to_end_uint8_t(struct dhcpclient_session *dhcp, uint8_t value) {
    MSG(dhcp)->options[OPTIONS_OFFSET] = value;
    LENGTH += sizeof(value);
}

static inline void _add_to_and_address(struct dhcpclient_session *dhcp, uip_ipaddr_t *address) {
    uip_ipaddr_copy(MSG(dhcp)->options + OPTIONS_OFFSET, address);
    LENGTH += sizeof(uip_ipaddr_t);
}

void dhcp_create_discover(struct dhcpclient_session *dhcp) {
    _create_message(dhcp);
    _add_message_type(dhcp, DHCP_MESSAGE_TYPE_DHCPDISCOVER);
    _add_request_options(dhcp);
    _add_end(dhcp);
}

void dhcp_process_offer(struct dhcpclient_session *dhcp) {
    uip_ipaddr(&dhcp->yiaddr, 192, 168, 7, 1);
    uip_ipaddr(&dhcp->siaddr, 192, 168, 7, 111);
    uip_ipaddr(&dhcp->netmask, 255, 255, 255, 0);
}

void dhcp_create_request(struct dhcpclient_session *dhcp) {
    _create_message(dhcp);
    _add_message_type(dhcp, DHCP_MESSAGE_TYPE_DHCPREQUEST);
    _add_server_id(dhcp);
    _add_request_ip_address(dhcp);
    _add_end(dhcp);
}

void dhcp_process_ack(struct dhcpclient_session *dhcp) {
}

static void _create_message(struct dhcpclient_session *dhcp) {
    MSG(dhcp)->op = DHCP_OP_BOOTREQUEST;
    MSG(dhcp)->htype = DHCP_HTYPE_ETHERNET_10;
    MSG(dhcp)->hlen = 6;
    MSG(dhcp)->hops = 0;
    memcpy(&MSG(dhcp)->xid, xid, sizeof(MSG(dhcp)->xid));
    MSG(dhcp)->secs = 0;
    MSG(dhcp)->flags = HTONS(DHCP_FLAGS_BROADCAST);
    memcpy(&MSG(dhcp)->ciaddr, &dhcp->yiaddr, sizeof(MSG(dhcp)->ciaddr));
    /* Always zero per rfc, server def. */
    memset(&MSG(dhcp)->yiaddr, 0, sizeof(MSG(dhcp)->yiaddr));
    memset(&MSG(dhcp)->siaddr, 0, sizeof(MSG(dhcp)->siaddr));
    memset(&MSG(dhcp)->giaddr, 0, sizeof(MSG(dhcp)->giaddr));
    memcpy(&MSG(dhcp)->chaddr, &uip_ethaddr, sizeof(struct uip_eth_addr));
    memset(MSG(dhcp)->chaddr + sizeof(struct uip_eth_addr), 0, sizeof(MSG(dhcp)->chaddr) - sizeof(struct uip_eth_addr));
    memset(MSG(dhcp)->sname, 0, sizeof(MSG(dhcp)->sname));
    memset(MSG(dhcp)->file, 0, sizeof(MSG(dhcp)->file));
    memcpy(MSG(dhcp)->options, magic_cookie, sizeof(magic_cookie));
    
    LENGTH = sizeof(struct dhcp_message) - DHCP_MESSAGE_OPTIONS_SIZE + sizeof(magic_cookie);
}

static void _add_message_type(struct dhcpclient_session *dhcp, uint8_t type) {
    _add_to_end_uint8_t(dhcp, DHCP_OPTION_MSG_TYPE);
    _add_to_end_uint8_t(dhcp, DHCP_OPTION_MSG_TYPE_LENGTH);
    _add_to_end_uint8_t(dhcp, type);
}

static void _add_server_id(struct dhcpclient_session *dhcp) {
    _add_to_end_uint8_t(dhcp, DHCP_OPTION_SERVER_ID);
    _add_to_end_uint8_t(dhcp, 4);
    _add_to_and_address(dhcp, &dhcp->siaddr);
}

static void _add_request_ip_address(struct dhcpclient_session *dhcp) {
    _add_to_end_uint8_t(dhcp, DHCP_OPTION_REQ_IPADDR);
    _add_to_end_uint8_t(dhcp, 4);
    _add_to_and_address(dhcp, &dhcp->yiaddr);
}

static void _add_request_options(struct dhcpclient_session *dhcp) {
    _add_to_end_uint8_t(dhcp, DHCP_OPTION_REQ_LIST);
    _add_to_end_uint8_t(dhcp, 3);
    _add_to_end_uint8_t(dhcp, DHCP_OPTION_SUBNET_MASK);
    _add_to_end_uint8_t(dhcp, DHCP_OPTION_ROUTER);
    _add_to_end_uint8_t(dhcp, DHCP_OPTION_DNS_SERVER);
}

static void _add_end(struct dhcpclient_session *dhcp) {
    _add_to_end_uint8_t(dhcp, DHCP_OPTION_END);
}
