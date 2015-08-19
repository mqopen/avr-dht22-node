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

#ifndef __DHCPFIELDS_H__
#define __DHCPFIELDS_H__

struct dhcp_lease_time {
    uint8_t x[4];
};

#define dhcp_lease_time_copy(src, dst)                                  \
                    do {                                                \
                        (dst).x[0] = (src).x[0];                        \
                        (dst).x[1] = (src).x[1];                        \
                        (dst).x[2] = (src).x[2];                        \
                        (dst).x[3] = (src).x[3];                        \
                    } while (0)

#endif
