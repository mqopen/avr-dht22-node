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
void mqttclient_process(void);
void mqttclient_appcall(void);

#endif
