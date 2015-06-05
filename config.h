#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "common.h"
#include "umqtt/umqtt.h"
#include "nethandler.h"

#define IP_ADDR0     192
#define IP_ADDR1     168
#define IP_ADDR2     1
#define IP_ADDR3     33

#define ETH_ADDR0    0x00
#define ETH_ADDR1    0xbd
#define ETH_ADDR2    0x3b
#define ETH_ADDR3    0x33
#define ETH_ADDR4    0x05
#define ETH_ADDR5    0x71

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
#define MQTT_BROKER_IP_ADDR2    1
#define MQTT_BROKER_IP_ADDR3    124

#define MQTT_BROKER_PORT        1883

#define MQTT_KEEP_ALIVE         30
#define MQTT_CLIENT_ID          "avr-mqtt-" STR(IP_ADDR0) "-" STR(IP_ADDR1) "-" STR(IP_ADDR2) "-" STR(IP_ADDR3)

typedef struct nethandler_state uip_tcp_appstate_t;
#define UIP_APPCALL nethandler_umqtt_appcall

#endif /*__CONFIG_H__*/
