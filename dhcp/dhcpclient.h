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

#ifndef __DHCPCLIENT_H__
#define __DHCPCLIENT_H__

#include "../uip/uip.h"

#define DHCPCLIENT_IP_BROADCAST_OCTET   255
#define DHCPCLIENT_IP_SOURCE_PORT       68
#define DHCPCLIENT_IP_DESTINATION_PORT  67

/*
 * Check if DHCP resolving is finished.
 */
#define dhcpclient_is_done()    (dhcpclient_state == DHCPCLIENT_STATE_FINISHED)

enum dhcpclient_state {
    DHCPCLIENT_STATE_INIT,
    DHCPCLIENT_STATE_INITIALIZED,
    DHCPCLIENT_STATE_DISCOVER_PENDING,
    DHCPCLIENT_STATE_DISCOVER_SENT,
    DHCPCLIENT_STATE_OFFER_RECEIVED,
    DHCPCLIENT_STATE_REQUEST_PENDING,
    DHCPCLIENT_STATE_REQUEST_SENT,
    DHCPCLIENT_STATE_ACK_RECEIVED,
    DHCPCLIENT_STATE_ADDRESS_CONFIGURED,
    DHCPCLIENT_STATE_FINISHED
};

extern enum dhcpclient_state dhcpclient_state;
extern struct dhcpsession dhcpclient_data;

void dhcpclient_init(void);
void dhcpclient_process(void);

void dhcpclient_appcall(void);
#endif
