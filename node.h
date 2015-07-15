#ifndef __NODE_H__
#define __NODE_H__

#include "umqtt/umqtt.h"

enum node_system_state {
    NODE_DHCP_QUERYING,
    NODE_DNS_QUERYING,
    NODE_MQTT,
};

struct node_appstate {
    struct umqtt_connection *conn;
};

struct node_udp_appstate {
};

extern enum node_system_state node_system_state;

void node_init(void);
void node_process(void);
void node_appcall(void);
void node_udp_appcall(void);

#endif
