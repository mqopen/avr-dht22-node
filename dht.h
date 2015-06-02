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

#define DHT_OK                      0
#define DHT_ERROR_CHECKSUM          1
#define DHT_ERROR_TIMEOUT           2
#define DHT_ERROR_CONNECT           3
#define DHT_ERROR_ACK_L             4
#define DHT_ERROR_ACK_H             5

struct dht_data {
    int16_t humidity;
    int16_t temperature;
};

struct dht_data_raw {
    uint8_t humidity_integral;
    uint8_t humidity_decimal;
    int8_t temperature_integral;
    uint8_t temperature_decimal;
};

extern struct dht_data dht_data;

void dht_init(void);

uint8_t dht_read(void);

#endif /* __DHT_H__ */
