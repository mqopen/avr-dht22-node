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

#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>
#include "uip/uip.h"
#include "uip/uiparp.h"
#include "enc28j60/network.h"
#include "common.h"
#include "node.h"

#define BUF (((struct uip_eth_hdr *)&uip_buf[0]))

/**
 * Send data out.
 */
static inline void _nethandler_send_out(void);

void nethandler_rx(void) {
    uip_len = network_read();
    if (uip_len > 0) {
        switch (ntohs(BUF->type)) {
            case UIP_ETHTYPE_IP:
                uip_arp_ipin();
                uip_input();
                _nethandler_send_out();
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
    uint8_t i;
    switch (node_system_state) {
        case NODE_MQTT:
            times(UIP_CONNS, i) {
                uip_periodic(i);
                _nethandler_send_out();
            }
            break;
        default:
            times(UIP_UDP_CONNS, i) {
                uip_udp_periodic(i);
                _nethandler_send_out();
            }
            break;
    }
}

static inline void _nethandler_send_out(void) {
     if (uip_len > 0) {
        uip_arp_out();
        network_send();
    }
}
