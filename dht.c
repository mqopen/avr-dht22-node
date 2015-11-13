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

/* Raw data sent by sensor */
struct dht_data_raw {
    uint8_t humidity_integral;
    uint8_t humidity_decimal;
    uint8_t temperature_integral;
    uint8_t temperature_decimal;
    uint8_t checksum;
} __attribute__((packed));

/* Data from last measurement. */
struct dht_data dht_data;

/* Current sensor reading. This structure could be build on the stack. */
static struct dht_data_raw received_data;

/* Translation unit private function prototypes */
uint8_t _dht_read(void);

void dht_init(void) {
    DHT_SDA_OUTPUT();
    DHT_SDA_HIGH();
}

enum dht_read_status _dht_read(void) {
    uint8_t mask = DHT_INITIAL_BITMASK;
    uint8_t idx = 0;
    uint8_t data = 0;
    uint8_t state;
    uint8_t previous_state = 0;
    uint8_t i;
    uint16_t zero_loop = DHT_TIMEOUT;
    uint16_t delta = 0;
    uint16_t loop_count;

    DHT_SDA_OUTPUT();
    DHT_SDA_LOW();

    /* Magic constant for now. */
    _delay_us(1100);

    DHT_SDA_INPUT();
    DHT_SDA_HIGH();

    loop_count = DHT_TIMEOUT * 2;
    while (bit_is_set(DHT_PIN, DHT_SDA)) {
        if (--loop_count == 0) return DHT_ERROR_CONNECT;
    }

    loop_count = DHT_TIMEOUT;
    while (bit_is_clear(DHT_PIN, DHT_SDA)) {
        if (--loop_count == 0) return DHT_ERROR_ACK_L;
    }

    loop_count = DHT_TIMEOUT;
    while (bit_is_set(DHT_PIN, DHT_SDA)) {
        if (--loop_count == 0) return DHT_ERROR_ACK_H;
    }

    uint8_t in;
    uint8_t j;
    uint8_t to_cnt;
    for (i = 0; i < DHT_DATA_BYTE_LEN; i++) {
        in = 0;
        for (j = 0; j < 8; j++) {
            to_cnt = 0;
            while (bit_is_set(DHT_PIN, DHT_SDA)) {
                _delay_us(2);
                if (to_cnt++ > 25)
                    return DHT_ERROR_TIMEOUT;
            }
            _delay_us(5);

            to_cnt = 0;
            while (bit_is_clear(DHT_PIN, DHT_SDA)) {
                _delay_us(2);
                if (to_cnt++ > 28)
                    return DHT_ERROR_TIMEOUT;
            }
            _delay_us(50);
            in <<= 1;
            if (bit_is_set(DHT_PIN, DHT_SDA)) {
                in |= 1;
            }
        }
        *(((uint8_t *) (&received_data)) + i) = in;
    }

    //loop_count = DHT_TIMEOUT;
    //for (i = DHT_BIT_COUNT; i != 0; ) {
        //state = bit_is_set(DHT_PIN, DHT_SDA);
        //if (state == 0 && previous_state != 0) {
            //if (i > DHT_LEADING_ZERO_BITS) {
                //zero_loop = min(zero_loop, loop_count);
                //delta = (DHT_TIMEOUT - zero_loop) / 4;
            //} else if (loop_count <= (zero_loop - delta)) {
                //data |= mask;
            //}
            //mask >>= 1;
            //if (mask == 0) {
                //mask = DHT_INITIAL_BITMASK;
                //*(((uint8_t *) (&received_data)) + idx) = data;
                //idx++;
                //data = 0;
            //}
            //--i;
            //loop_count = DHT_TIMEOUT;
        //}
        //previous_state = state;
        //if (--loop_count == 0)
            //return DHT_ERROR_TIMEOUT;
    //}
    DHT_SDA_OUTPUT();
    DHT_SDA_HIGH();
    return DHT_OK;
}

enum dht_read_status dht_read(void) {
    enum dht_read_status result = _dht_read();
    received_data.humidity_integral &= 0x03;
    received_data.temperature_integral &= 0x83;

    /* Checksum */
    uint8_t sum = received_data.humidity_integral       +
                    received_data.humidity_decimal      +
                    received_data.temperature_integral  +
                    received_data.temperature_decimal;
    if (received_data.checksum != sum) {
        return DHT_ERROR_CHECKSUM;
    }

    dht_data.humidity = (received_data.humidity_integral << 8) | received_data.humidity_decimal;
    dht_data.temperature = (received_data.temperature_integral << 8) | received_data.temperature_decimal;

    /* Check negative temperature */
    if(received_data.temperature_integral & DHT_NEGATIVE_TEMPERATURE_BITMASK)
        dht_data.temperature = -dht_data.temperature;

    return result;
}
