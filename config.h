/*
 * Copyright (C) Ivo Slanina <ivo.slanina@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <avr/io.h>
#include "node.h"
#include "common.h"

#define CONFIG_DEBUG    1

#define ETH_ADDR0       0x76
#define ETH_ADDR1       0xe6
#define ETH_ADDR2       0xe2
#define ETH_ADDR3       0x18
#define ETH_ADDR4       0x2f
#define ETH_ADDR5       0x44


/* IP configuration. */
#define CONFIG_DHCP     0
#if !(CONFIG_DHCP)
#define CONFIG_IP_ADDR0 192
#define CONFIG_IP_ADDR1 168
#define CONFIG_IP_ADDR2 1
#define CONFIG_IP_ADDR3 41

#define CONFIG_NETMASK0 255
#define CONFIG_NETMASK1 255
#define CONFIG_NETMASK2 252
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
#define MQTT_BROKER_IP_ADDR2    1
#define MQTT_BROKER_IP_ADDR3    209

#define MQTT_BROKER_PORT        1883

#define MQTT_TOPIC_TEMPERATURE  "sensors/temperature"
#define MQTT_TOPIC_HUMIDITY     "sensors/humidity"

#define MQTT_PUBLISH_PERIOD     2

#define MQTT_KEEP_ALIVE         30
#define MQTT_CLIENT_ID          "avr-mqtt-cr"

/* MQTT node presence */
#define MQTT_NODE_PRESENCE      1
#define MQTT_NODE_PRESENCE_TOPIC    "presence/" STR(MQTT_CLIENT_ID)

/* TCP connections. */
typedef struct node_appstate uip_tcp_appstate_t;
#define UIP_APPCALL node_appcall

/* UDP connections. */
typedef struct node_udp_appstate uip_udp_appstate_t;
#define UIP_UDP_APPCALL node_udp_appcall

#endif /*__CONFIG_H__*/
