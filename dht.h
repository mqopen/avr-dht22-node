/* _
 * Header file for the DHT11 driver
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

#ifndef __DHT_H__
#define __DHT_H__

struct dht_data {
    uint16_t humidity;
    int16_t temperature;
};

enum dht_read_status {
    DHT_OK,
    DHT_ERROR_CHECKSUM,
    DHT_ERROR_TIMEOUT,
    DHT_ERROR_CONNECT,
    DHT_ERROR_ACK_L,
    DHT_ERROR_ACK_H
};

extern struct dht_data dht_data;

void dht_init(void);

enum dht_read_status dht_read(void);

#endif /* __DHT_H__ */
