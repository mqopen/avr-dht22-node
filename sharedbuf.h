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

#ifndef __SHAREDBUF_H__
#define __SHAREDBUF_H__

#include "config.h"
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
