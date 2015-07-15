#ifndef __MQTTCLIENT_H__
#define __MQTTCLIENT_H__

enum mqttclient_state {
    MQTTCLIENT_BROKER_DISCONNECTED,
    MQTTCLIENT_BROKER_DISCONNECTED_WAIT,
    MQTTCLIENT_BROKER_CONNECTING,
    MQTTCLIENT_BROKER_CONNECTION_ESTABLISHED,
    MQTTCLIENT_BROKER_DISCONNECTING,
};

void mqttclient_init(void);
void mqttclient_notify_broker_unreachable(void);
void mqttclient_process(void);
void mqttclient_appcall(void);

#endif
