#ifndef __DHCP_H__
#define __DHCP_H__

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

enum dhcp_state {
    DHCP_STATE_INIT,
    DHCP_STATE_DISCOVER_SENT,
    DHCP_STATE_OFFER_RECEIVED,
    DHCP_STATE_REQUEST_SENT,
    DHCP_STATE_ACK_RECEIVED
};

struct dhcp_message {
    uint8_t op;
    uint8_t htype;
    uint8_t hlen;
    uint8_t hops;
    uint32_t xid;
    uint16_t secs;
    uint16_t flags;
    uint32_t ciaddr;
    uint32_t yiaddr;
    uint32_t siaddr;
    uint32_t giaddr;
    uint8_t chaddr[16];
    uint8_t sname[64];
    uint8_t file[128];
    uint8_t options[312];
};

#endif
