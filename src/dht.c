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

#include <util/delay.h>
#include "common.h"
#include "config.h"
#include "dht.h"

#define DHT_TIMEOUT                         400
#define DHT_INITIAL_BITMASK                 0x80
#define DHT_NEGATIVE_TEMPERATURE_BITMASK    0x80

#define DHT_SDA_OUTPUT()    (DHT_DDR |= _BV(DHT_SDA))
#define DHT_SDA_INPUT()     (DHT_DDR &= ~(_BV(DHT_SDA)))
#define DHT_SDA_HIGH()      (DHT_PORT |= _BV(DHT_SDA))
#define DHT_SDA_LOW()       (DHT_PORT &= ~(_BV(DHT_SDA)))

/** Raw data sent by sensor */
struct dht_data_raw {
    uint8_t humidity_msb;
    uint8_t humidity_lsb;
    uint8_t temperature_msb;
    uint8_t temperature_lsb;
    uint8_t checksum;
} __attribute__((__packed__));

/* Data from last measurement. */
struct dht_data dht_data;

/* Translation unit private function prototypes */
uint8_t _dht_read(void *buf);

void dht_init(void) {
    DHT_SDA_OUTPUT();
    DHT_SDA_HIGH();
}

enum dht_read_status _dht_read(void *buf) {
    uint16_t timeout;
    uint8_t result;
    uint8_t i;
    uint8_t j;

    /* Reset port. */
    DHT_SDA_OUTPUT();
    DHT_SDA_HIGH();
    _delay_ms(100);

    /* Send request. */
    DHT_SDA_LOW();
    _delay_us(1100);
    DHT_SDA_HIGH();
    DHT_SDA_INPUT();
    _delay_us(40);

    if (bit_is_set(DHT_PIN, DHT_SDA))
        return DHT_ERROR_CONNECT;
    _delay_us(80);

    if (bit_is_clear(DHT_PIN, DHT_SDA))
        return DHT_ERROR_ACK;
    _delay_us(80);

    for (j = 0; j < DHT_DATA_BYTE_LEN; j++) {
        result = 0;
        for (i = 0; i < 8; i++) {
            timeout = 0;
            while (bit_is_clear(DHT_PIN, DHT_SDA)) {
                if (timeout++ > 200)
                    return DHT_ERROR_TIMEOUT;
            }

            _delay_us(30);
            if (bit_is_set(DHT_PIN, DHT_SDA))
                result |= _BV(7 - i);

            timeout = 0;
            while (bit_is_set(DHT_PIN, DHT_SDA)) {
                if(timeout++ > 200)
                    return DHT_ERROR_TIMEOUT;
            }
        }
        *(((uint8_t *) buf) + j) = result;
    }

    /* Reset port. */
    DHT_SDA_OUTPUT();
    DHT_SDA_HIGH();
    _delay_ms(100);

    return DHT_OK;
}

enum dht_read_status dht_read(void) {
    struct dht_data_raw _raw_data;
    enum dht_read_status result = _dht_read(&_raw_data);
    if (result != DHT_OK)
        return result;

    /* Checksum */
    uint8_t sum = _raw_data.humidity_msb        +
                    _raw_data.humidity_lsb      +
                    _raw_data.temperature_msb   +
                    _raw_data.temperature_lsb;
    if (_raw_data.checksum != sum) {
        return DHT_ERROR_CHECKSUM;
    }

    dht_data.humidity = (_raw_data.humidity_msb << 8) | _raw_data.humidity_lsb;
    dht_data.temperature = ((_raw_data.temperature_msb & 0x7f) << 8) | _raw_data.temperature_lsb;

    if ((_raw_data.temperature_msb & 0x80)) {
        dht_data.temperature = -dht_data.temperature;
    }

    return result;
}
