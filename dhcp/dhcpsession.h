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

#ifndef __DHCPSESSION_H__
#define __DHCPSESSION_H__

#include "dhcpfields.h"
#include "../uip/uip.h"

struct dhcpsession {
    uint8_t *buffer;
    uint16_t length;
    // TODO: remove duplicit code (perhaps create own data type for xid)
    uint8_t xid[4];
    uip_ipaddr_t client_address;
    uip_ipaddr_t server_address;
    uip_ipaddr_t netmask;
    uip_ipaddr_t dns;
    struct dhcp_lease_time lease_time;
};

#endif
