#include <string.h>
#include "uip/uip.h"
#include "uip/uiparp.h"
#include "sharedbuf.h"
#include "node.h"
#include "dhcp.h"

#define BUFFER  (sharedbuf + SHAREDBUF_DHCP_OFFSET)
#define MSG     ((struct dhcp_message *) BUFFER)
#define UPDATE_SEND_LENGTH()    node_send_udp_length = message_end - BUFFER

static const uint8_t xid[] = {0xad, 0xde, 0x12, 0x23};
static const uint8_t magic_cookie[] = {99, 130, 83, 99};

static uint8_t *message_end;

enum dhcp_state dhcp_state;

void dhcp_init(void) {
    dhcp_state = DHCP_STATE_INIT;
}

static void create_message(void) {
    MSG->op = DHCP_OP_BOOTREQUEST;
    MSG->htype = DHCP_HTYPE_ETHERNET_10;
    MSG->hlen = 6;
    MSG->hops = 0;
    memcpy(&MSG->xid, xid, sizeof(MSG->xid));
    MSG->secs = 0;
    MSG->flags = HTONS(DHCP_FLAGS_BROADCAST);
    memcpy(&MSG->ciaddr, uip_hostaddr, sizeof(MSG->ciaddr));
    /* Always zero per rfc, server def. */
    memset(&MSG->yiaddr, 0, sizeof(MSG->yiaddr));
    memset(&MSG->siaddr, 0, sizeof(MSG->siaddr));
    memset(&MSG->giaddr, 0, sizeof(MSG->giaddr));
    memcpy(&MSG->chaddr, &uip_ethaddr, sizeof(struct uip_eth_addr));
    memset(&MSG->chaddr[sizeof(struct uip_eth_addr)], 0, sizeof(MSG->chaddr) - sizeof(struct uip_eth_addr));
    memset(MSG->sname, 0, sizeof(MSG->sname));
    memset(MSG->file, 0, sizeof(MSG->file));
    memcpy(MSG->options, magic_cookie, sizeof(magic_cookie));
    
    message_end = MSG->options + sizeof(magic_cookie);
}

static void add_message_type(uint8_t type) {
    *message_end++ = DHCP_OPTION_MSG_TYPE;
    *message_end++ = DHCP_OPTION_MSG_TYPE_LENGTH;
    *message_end++ = type;
}

static void add_server_id(void) {
}

static void add_request_ip_address(void) {
}

static void add_request_options(void) {
    *message_end++ = DHCP_OPTION_REQ_LIST;
    *message_end++ = 3;
    *message_end++ = DHCP_OPTION_SUBNET_MASK;
    *message_end++ = DHCP_OPTION_ROUTER;
    *message_end++ = DHCP_OPTION_DNS_SERVER;
}

static void add_end(void) {
    *message_end++ = DHCP_OPTION_END;
}

void dhcp_create_discover(void) {
    create_message();
    add_message_type(DHCP_MESSAGE_TYPE_DHCPDISCOVER);
    add_request_options();
    add_end();
    UPDATE_SEND_LENGTH();
}

static void request(void) {
    create_message();
    add_message_type(DHCP_MESSAGE_TYPE_DHCPREQUEST);
    add_server_id();
    add_request_ip_address();
    UPDATE_SEND_LENGTH();
}
