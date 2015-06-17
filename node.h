#ifndef __NODE_H__
#define __NODE_H__

#include "umqtt/umqtt.h"

enum node_system_state {
    NODE_DHCP_QUERYING,
    NODE_DNS_QUERYING,
    NODE_BROKER_DISCONNECTED,
    NODE_BROKER_DISCONNECTED_WAIT,
    NODE_BROKER_CONNECTING,
    NODE_BROKER_CONNECTION_ESTABLISHED
};

struct node_appstate {
    struct umqtt_connection *conn;
};

struct node_udp_appstate {
};

extern struct umqtt_connection mqtt;

extern enum node_system_state node_system_state;

/* Length of bytes which waits for UDP transmitt. */
extern uint16_t node_send_udp_length;

void node_init(void);
void node_process(void);
void node_notify_broker_unreachable(void);

void node_appcall(void);
void node_udp_appcall(void);

#endif
