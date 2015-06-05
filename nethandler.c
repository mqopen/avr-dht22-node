/* 
 * This fil handles various network related tasks
 *
 * Copyright (C) Josef Gajdusek <atx@atx.name>
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
 * 
 * */

#include <avr/interrupt.h>
#include <string.h>

#include "common.h"
#include "uip/uip.h"
#include "uip/uiparp.h"
#include "enc28j60/network.h"

#include "uart.h"

#define BUF (((struct uip_eth_hdr *)&uip_buf[0]))
#pragma GCC diagnostic ignored "-Wstrict-aliasing"

void nethandler_rx(void) {
    uip_len = network_read();
    if (uip_len > 0) {
        switch (ntohs(BUF->type)) {
        case UIP_ETHTYPE_IP:
            uip_arp_ipin();
            uip_input();
            if (uip_len > 0) {
                uip_arp_out();
                network_send();
            }
        break;
        case UIP_ETHTYPE_ARP:
            uip_arp_arpin();
            if (uip_len > 0)
                network_send();
        break;
        }
    }
}

void nethandler_periodic(void) {
    int i;
    times(UIP_CONNS, i) {
        uip_periodic(i);
        if (uip_len > 0) {
            uip_arp_out();
            network_send();
        }
    }
}

void nethandler_periodic_arp(void) {
    uip_arp_timer();
}

void nethandler_umqtt_init(struct umqtt_connection *conn) {
    struct uip_conn *uc;
    uip_ipaddr_t ip;

    uip_ipaddr(&ip, MQTT_BROKER_IP_ADDR0, MQTT_BROKER_IP_ADDR1, MQTT_BROKER_IP_ADDR2, MQTT_BROKER_IP_ADDR3);
    uc = uip_connect(&ip, htons(MQTT_BROKER_PORT));
    if (uc == NULL)
        return;

    umqtt_init(conn);
    umqtt_circ_init(&conn->txbuff);
    umqtt_circ_init(&conn->rxbuff);

    umqtt_connect(conn, MQTT_KEEP_ALIVE, MQTT_CLIENT_ID);

    uc->appstate.conn = conn;
}

void nethandler_umqtt_appcall(void) {
    struct umqtt_connection *conn = uip_conn->appstate.conn;
    uint8_t buff[uip_mss() > (uint16_t) conn->txbuff.datalen ? (uint16_t) conn->txbuff.datalen : uip_mss()];
    int ret;
    
    if (uip_poll() || uip_acked()) {
        ret = umqtt_circ_pop(&conn->txbuff, buff, sizeof(buff));
        if (!ret)
            return;
        uip_send(buff, ret);
    }
}

