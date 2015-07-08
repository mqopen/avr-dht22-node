#ifndef __DHCP_H__
#define __DHCP_H__

#include "dhcpclient.h"
#include "common.h"

#define DHCP_FLAGS_BROADCAST        0x8000
#define DHCP_MESSAGE_CHADDR_SIZE    16
#define DHCP_MESSAGE_SNAME_SIZE     64
#define DHCP_MESSAGE_FILE_SIZE      128
#define DHCP_MESSAGE_OPTIONS_SIZE   312

#define DHCP_OPTION_MSG_TYPE_LENGTH     1

#define dhcp_lease_time_copy(dest, src) do {                                \
                        ((uint16_t *)(dest))[0] = ((uint16_t *)(src))[0];   \
                        ((uint16_t *)(dest))[1] = ((uint16_t *)(src))[1];   \
                    } while (0)

struct dhcp_message {
    uint8_t op;
    uint8_t htype;
    uint8_t hlen;
    uint8_t hops;
    uint32_t xid;
    uint16_t secs;
    uint16_t flags;
    uip_ipaddr_t ciaddr;
    uip_ipaddr_t yiaddr;
    uip_ipaddr_t siaddr;
    uip_ipaddr_t giaddr;
    uint8_t chaddr[DHCP_MESSAGE_CHADDR_SIZE];
    uint8_t sname[DHCP_MESSAGE_SNAME_SIZE];
    uint8_t file[DHCP_MESSAGE_FILE_SIZE];
    uint8_t options[DHCP_MESSAGE_OPTIONS_SIZE];
};

enum dhcp_op {
    DHCP_OP_BOOTREQUEST         = 1,
    DHCP_OP_BOOTREPLY           = 2
};

enum dhcp_htype {
    DHCP_HTYPE_ETHERNET_10      = 1,
    DHCP_HTYPE_ETHERNET_3       = 2,
    DHCP_HTYPE_AX_25            = 3,
    DHCP_HTYPE_PRONET           = 4,
    DHCP_HTYPE_CHAOS            = 5,
    DHCP_HTYPE_IEEE_802         = 6,
    DHCP_HTYPE_ARCNET           = 7,
    DHCP_HTYPE_HYPERCHANNEL     = 8,
    DHCP_HTYPE_LANSTAR          = 9,
    DHCP_HTYPE_AUTONET          = 10,
    DHCP_HTYPE_LOCAL_TALK       = 11,
    DHCP_HTYPE_LOCAL_NET        = 12,
    DHCP_HTYPE_ULTRA_LINK       = 13,
    DHCP_HTYPE_SMDS             = 14,
    DHCP_HTYPE_FRAME_RELAY      = 15,
    DHCP_HTYPE_ATM_16           = 16,
    DHCP_HTYPE_HDLC             = 17,
    DHCP_HTYPE_FIBRE_CHANNEL    = 18,
    DHCP_HTYPE_ATM_19           = 19,
    DHCP_HTYPE_SERIAL_LINE      = 20,
    DHCP_HTYPE_ATM_21           = 21
};

enum dhcp_message_type {
    DHCP_MESSAGE_TYPE_DHCPDISCOVER  = 1,
    DHCP_MESSAGE_TYPE_DHCPOFFER     = 2,
    DHCP_MESSAGE_TYPE_DHCPREQUEST   = 3,
    DHCP_MESSAGE_TYPE_DHCPDECLINE   = 4,
    DHCP_MESSAGE_TYPE_DHCPACK       = 5,
    DHCP_MESSAGE_TYPE_DHCPNAK       = 7,
    DHCP_MESSAGE_TYPE_DHCPRELEASE   = 8
};

enum dhcp_option {
    DHCP_OPTION_SUBNET_MASK     = 1,
    DHCP_OPTION_ROUTER          = 3,
    DHCP_OPTION_DNS_SERVER      = 6,
    DHCP_OPTION_NTP_SERVER      = 42,
    DHCP_OPTION_REQ_IPADDR      = 50,
    DHCP_OPTION_LEASE_TIME      = 51,
    DHCP_OPTION_MSG_TYPE        = 53,
    DHCP_OPTION_SERVER_ID       = 54,
    DHCP_OPTION_REQ_LIST        = 55,
    DHCP_OPTION_END             = 255
};

struct dhcp_option_header {
    enum dhcp_option option;
    uint8_t option_length;
};

struct dhcp_option_address {
    struct dhcp_option_header header;
    uip_ipaddr_t address;
};

struct dhcp_option_lease_time {
    struct dhcp_option_header header;
    dhcp_lease_time_t lease_time;
};

void dhcp_create_discover(struct dhcpclient_session *dhcp);
void dhcp_process_offer(struct dhcpclient_session *dhcp);
void dhcp_create_request(struct dhcpclient_session *dhcp);
void dhcp_process_ack(struct dhcpclient_session *dhcp);
#endif
