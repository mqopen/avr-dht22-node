#include <string.h>
#include "../common.h"
#include "../uip/uiparp.h"
#include "dhcp.h"

#include "../uart.h"

#define MSG(__d)                    ((struct dhcp_message *) __d->buffer)
#define OPTIONS_OFFSET(__d)         (__d->length - sizeof(struct dhcp_message) + member_size(struct dhcp_message, options))

/* Static function prototypes. */
static void _create_message(struct dhcpsession *dhcp);
static void _add_message_type(struct dhcpsession *dhcp, uint8_t type);
static void _add_server_id(struct dhcpsession *dhcp);
static void _add_request_ip_address(struct dhcpsession *dhcp);
static void _add_request_options(struct dhcpsession *dhcp);
static void _add_end(struct dhcpsession *dhcp);
static inline void _parse_client_address(struct dhcpsession *dhcp);
static bool _parse_server_identifier(struct dhcpsession *dhcp);
static bool _parse_netmask(struct dhcpsession *dhcp);
static bool _parse_dns_server(struct dhcpsession *dhcp);
static bool _parse_lease_time(struct dhcpsession *dhcp);
static inline void _add_to_end_uint8_t(struct dhcpsession *dhcp, uint8_t value);
static inline void _add_to_and_address(struct dhcpsession *dhcp, uip_ipaddr_t *address);
static void *_find_option(struct dhcp_message *message, uint16_t message_length, enum dhcp_option option);

void dhcp_create_discover(struct dhcpsession *dhcp) {
    _create_message(dhcp);
    _add_message_type(dhcp, DHCP_MESSAGE_TYPE_DHCPDISCOVER);
    _add_request_options(dhcp);
    _add_end(dhcp);
}

bool dhcp_process_offer(struct dhcpsession *dhcp) {
    /* Check if message is reply. */
    if ((enum dhcp_op) MSG(dhcp)->op != DHCP_OP_BOOTREPLY)
        return false;
    _parse_client_address(dhcp);
    if (!_parse_server_identifier(dhcp))
        return false;
    if (!_parse_netmask(dhcp))
        return false;
    if (!_parse_dns_server(dhcp))
        return false;
    if (!_parse_lease_time(dhcp))
        return false;
    return true;
}

void dhcp_create_request(struct dhcpsession *dhcp) {
    _create_message(dhcp);
    _add_message_type(dhcp, DHCP_MESSAGE_TYPE_DHCPREQUEST);
    _add_server_id(dhcp);
    _add_request_ip_address(dhcp);
    _add_end(dhcp);
}

bool dhcp_process_ack(struct dhcpsession *dhcp) {
    /* Check if message is reply. */
    if ((enum dhcp_op) MSG(dhcp)->op != DHCP_OP_BOOTREPLY)
        return false;
    return true;
}

static void _create_message(struct dhcpsession *dhcp) {
    MSG(dhcp)->op = DHCP_OP_BOOTREQUEST;
    MSG(dhcp)->htype = DHCP_HTYPE_ETHERNET_10;
    MSG(dhcp)->hlen = 6;
    MSG(dhcp)->hops = 0;
    memcpy(&MSG(dhcp)->xid, dhcp->xid, sizeof(MSG(dhcp)->xid));
    MSG(dhcp)->secs = 0;
    MSG(dhcp)->flags = HTONS(DHCP_FLAGS_BROADCAST);
    memcpy(&MSG(dhcp)->ciaddr, &dhcp->client_address, sizeof(MSG(dhcp)->ciaddr));
    /* Always zero per RFC, server def. */
    memset(&MSG(dhcp)->yiaddr, 0, sizeof(MSG(dhcp)->yiaddr));
    memset(&MSG(dhcp)->siaddr, 0, sizeof(MSG(dhcp)->siaddr));
    memset(&MSG(dhcp)->giaddr, 0, sizeof(MSG(dhcp)->giaddr));
    memcpy(&MSG(dhcp)->chaddr, &uip_ethaddr, sizeof(struct uip_eth_addr));
    memset(MSG(dhcp)->chaddr + sizeof(struct uip_eth_addr), '\x00', sizeof(MSG(dhcp)->chaddr) - sizeof(struct uip_eth_addr));
    memset(MSG(dhcp)->sname, 0, sizeof(MSG(dhcp)->sname));
    memset(MSG(dhcp)->file, 0, sizeof(MSG(dhcp)->file));
    memcpy(MSG(dhcp)->options, magic_cookie, sizeof(magic_cookie));
    
    dhcp->length = sizeof(struct dhcp_message) - member_size(struct dhcp_message, options) + sizeof(magic_cookie);
}

static void _add_message_type(struct dhcpsession *dhcp, uint8_t type) {
    _add_to_end_uint8_t(dhcp, DHCP_OPTION_MSG_TYPE);
    _add_to_end_uint8_t(dhcp, DHCP_OPTION_MSG_TYPE_LENGTH);
    _add_to_end_uint8_t(dhcp, type);
}

static void _add_server_id(struct dhcpsession *dhcp) {
    _add_to_end_uint8_t(dhcp, DHCP_OPTION_SERVER_ID);
    _add_to_end_uint8_t(dhcp, 4);
    _add_to_and_address(dhcp, &dhcp->server_address);
}

static void _add_request_ip_address(struct dhcpsession *dhcp) {
    _add_to_end_uint8_t(dhcp, DHCP_OPTION_REQ_IPADDR);
    _add_to_end_uint8_t(dhcp, 4);
    _add_to_and_address(dhcp, &dhcp->client_address);
}

static void _add_request_options(struct dhcpsession *dhcp) {
    _add_to_end_uint8_t(dhcp, DHCP_OPTION_REQ_LIST);
    _add_to_end_uint8_t(dhcp, 3);
    _add_to_end_uint8_t(dhcp, DHCP_OPTION_SUBNET_MASK);
    _add_to_end_uint8_t(dhcp, DHCP_OPTION_ROUTER);
    _add_to_end_uint8_t(dhcp, DHCP_OPTION_DNS_SERVER);
}

static void _add_end(struct dhcpsession *dhcp) {
    _add_to_end_uint8_t(dhcp, DHCP_OPTION_END);
}

static inline void _parse_client_address(struct dhcpsession *dhcp) {
    uip_ipaddr_copy(&dhcp->client_address, &MSG(dhcp)->yiaddr);
}

static bool _parse_server_identifier(struct dhcpsession *dhcp) {
    struct dhcp_option_address *server_identifier_opt = _find_option(MSG(dhcp), dhcp->length, DHCP_OPTION_SERVER_ID);
    if (server_identifier_opt == NULL)
        return false;
    uip_ipaddr_copy(&dhcp->server_address, &server_identifier_opt->address);
    return true;
}

static bool _parse_netmask(struct dhcpsession *dhcp) {
    struct dhcp_option_address *netmask_opt = _find_option(MSG(dhcp), dhcp->length, DHCP_OPTION_SUBNET_MASK);
    if (netmask_opt == NULL)
        return false;
    uip_ipaddr_copy(&dhcp->netmask, &netmask_opt->address);
    return true;
}

static bool _parse_dns_server(struct dhcpsession *dhcp) {
    struct dhcp_option_address *dns_server_opt = _find_option(MSG(dhcp), dhcp->length, DHCP_OPTION_DNS_SERVER);
    if (dns_server_opt == NULL)
        return false;
    uip_ipaddr_copy(&dhcp->dns, &dns_server_opt->address);
    return true;
}

static bool _parse_lease_time(struct dhcpsession *dhcp) {
    struct dhcp_option_lease_time *lease_time_opt = _find_option(MSG(dhcp), dhcp->length, DHCP_OPTION_LEASE_TIME);
    if (lease_time_opt == NULL)
        return false;
    dhcp_lease_time_copy(lease_time_opt->lease_time, dhcp->lease_time);
    return true;
}

static inline void _add_to_end_uint8_t(struct dhcpsession *dhcp, uint8_t value) {
    MSG(dhcp)->options[OPTIONS_OFFSET(dhcp)] = value;
    dhcp->length += sizeof(value);
}

static inline void _add_to_and_address(struct dhcpsession *dhcp, uip_ipaddr_t *address) {
    uip_ipaddr_copy(MSG(dhcp)->options + OPTIONS_OFFSET(dhcp), address);
    dhcp->length += sizeof(*address);
}

static void *_find_option(struct dhcp_message *message, uint16_t message_length, enum dhcp_option option) {
    /* Skip 4 byte of magic cookie. */
    uint8_t i = 4;
    const uint16_t l = message_length - sizeof(struct dhcp_message) + member_size(struct dhcp_message, options);
    while (i < l) {
        enum dhcp_option opt = (enum dhcp_option) message->options[i];
        uint8_t opt_len = (uint8_t) message->options[i + 1];
        if (opt == option)
            return (message->options) + i;
        else
            /* Option key + option length indicator + option value length. */
            i += (2 + opt_len);
    }
    return NULL;
}
