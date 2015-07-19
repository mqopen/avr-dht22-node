#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <avr/io.h>
#include "node.h"

#define CONFIG_DEBUG    1

#define ETH_ADDR0       0x76
#define ETH_ADDR1       0xe6
#define ETH_ADDR2       0xe2
#define ETH_ADDR3       0x18
#define ETH_ADDR4       0x2f
#define ETH_ADDR5       0x44


/* IP configuration. */
#define CONFIG_DHCP     1
#if !(CONFIG_DHCP)
#define CONFIG_IP_ADDR0 192
#define CONFIG_IP_ADDR1 168
#define CONFIG_IP_ADDR2 7
#define CONFIG_IP_ADDR3 1

#define CONFIG_NETMASK0 255
#define CONFIG_NETMASK1 255
#define CONFIG_NETMASK2 255
#define CONFIG_NETMASK3 0
#endif

/* SPI enc28j60 interface configuration. */
#define ENC28J60_SPI_PORT       PORTB
#define ENC28J60_SPI_DDR        DDRB
#define ENC28J60_SPI_SCK        PB5
#define ENC28J60_SPI_MOSI       PB3
#define ENC28J60_SPI_MISO       PB4
#define ENC28J60_SPI_SS         PB2
#define ENC28J60_CONTROL_PORT   PORTB
#define ENC28J60_CONTROL_DDR    DDRB
#define ENC28J60_CONTROL_CS     PB0

/* OneWire DHT-22 interface configuration. */
#define DHT_PORT                PORTB
#define DHT_DDR                 DDRB
#define DHT_PIN                 PINB
#define DHT_SDA                 PB1

/* MQTT configuration. */
#define MQTT_BROKER_IP_ADDR0    192
#define MQTT_BROKER_IP_ADDR1    168
#define MQTT_BROKER_IP_ADDR2    7
#define MQTT_BROKER_IP_ADDR3    111

#define MQTT_BROKER_PORT        1883

#define MQTT_TOPIC_TEMPERATURE  "sensors/temperature/test"
#define MQTT_TOPIC_HUMIDITY     "sensors/humidity/test"

#define MQTT_KEEP_ALIVE         30
#define MQTT_CLIENT_ID          "avr-mqtt-test"

/* TCP connections. */
typedef struct node_appstate uip_tcp_appstate_t;
#define UIP_APPCALL node_appcall

/* UDP connections. */
typedef struct node_udp_appstate uip_udp_appstate_t;
#define UIP_UDP_APPCALL node_udp_appcall

#endif /*__CONFIG_H__*/
