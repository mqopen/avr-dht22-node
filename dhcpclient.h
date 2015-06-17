#ifndef __DHCPCLIENT_H__
#define __DHCPCLIENT_H__

#define DHCPCLIENT_IP_BROADCAST_OCTET   255
#define DHCPCLIENT_IP_SOURCE_PORT       68
#define DHCPCLIENT_IP_DESTINATION_PORT  67

struct dhcpclient_data {
    uint8_t *buffer;
    uint16_t length;
};

enum dhcpclient_state {
    DHCPCLIENT_STATE_INIT,
    DHCPCLIENT_STATE_DISCOVER_PENDING,
    DHCPCLIENT_STATE_DISCOVER_SENT,
    DHCPCLIENT_STATE_OFFER_RECEIVED,
    DHCPCLIENT_STATE_REQUEST_PENDING,
    DHCPCLIENT_STATE_REQUEST_SENT,
    DHCPCLIENT_STATE_ACK_RECEIVED
};

extern enum dhcpclient_state dhcpclient_state;

void dhcpclient_init(void);
void dhcpclient_process(void);
void dhcpclient_handle_message(void);

void dhcpclient_appcall(void);
#endif
