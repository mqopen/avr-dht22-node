/*
 * Shared fuffer header file
 */
#ifndef __SHAREDBUF_H__
#define __SHAREDBUF_H__

#if CONFIG_DHCP
#include "dhcp/dhcp.h"
#endif

#define SHAREDBUF_NODE_SEND_BUFFER_SIZE 100
#define SHAREDBUF_NODE_UMQTT_RX_SIZE    150
#define SHAREDBUF_NODE_UMQTT_TX_SIZE    200

#if CONFIG_DHCP
struct sharedbuf_dhcp {
    uint8_t buffer[sizeof(struct dhcp_message)];
};
#endif

struct sharedbuf_mqtt {
    uint8_t send_buffer[SHAREDBUF_NODE_SEND_BUFFER_SIZE];
    uint8_t mqtt_rx[SHAREDBUF_NODE_UMQTT_RX_SIZE];
    uint8_t mqtt_tx[SHAREDBUF_NODE_UMQTT_TX_SIZE];
};

union sharedbuf_buffer {
#if CONFIG_DHCP
    struct sharedbuf_dhcp dhcp;
#endif
    struct sharedbuf_mqtt mqtt;
};

extern union sharedbuf_buffer sharedbuf;

void sharedbuf_clear(void);

#endif
