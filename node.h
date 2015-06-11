#ifndef __NODE_H__
#define __NODE_H__

enum system_state {
    dhcp_querying,
    dns_querying,
    broker_connecting
};

extern enum system_state system_state;

void node_process(void);

#endif
