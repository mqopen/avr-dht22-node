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

#ifndef __NODE_H__
#define __NODE_H__

#include "config.h"
#include "umqtt/umqtt.h"

#if CONFIG_DHCP
#define NODE_STATE_INIT NODE_DHCP_QUERYING
#else
#define NODE_STATE_INIT NODE_MQTT
#endif

enum node_system_state {
#if CONFIG_DHCP
    NODE_DHCP_QUERYING,
#endif
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
