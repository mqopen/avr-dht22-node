/* 
 * Driver for DHT11 humidity sensor
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

#include <util/delay.h>
#include "common.h"
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
};

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

uint8_t _dht_read(void) {
    uint8_t mask = DHT_INITIAL_BITMASK;
    uint8_t idx = 0;
    uint8_t data = 0;
    uint8_t state;
    uint8_t previous_state = 0;
    uint8_t i;
    uint16_t zero_loop = DHT_TIMEOUT;
    uint16_t delta = 0;
    uint16_t loop_count;

    /* Magic constant for now. */
    uint8_t leading_zero_bits = 36;
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
    
    loop_count = DHT_TIMEOUT;
    for (i = 40; i != 0; ) {
        state = bit_is_set(DHT_PIN, DHT_SDA);
        if (state == 0 && previous_state != 0) {
            if (i > leading_zero_bits) {
                zero_loop = min(zero_loop, loop_count);
                delta = (DHT_TIMEOUT - zero_loop) / 4;
            } else if (loop_count <= (zero_loop - delta)) {
                data |= mask;
            }
            mask >>= 1;
            if (mask == 0) {
                mask = DHT_INITIAL_BITMASK;
                *(((uint8_t *) (&received_data)) + idx) = data;
                idx++;
                data = 0;
            }
            --i;
            loop_count = DHT_TIMEOUT;
        }
        previous_state = state;
        if (--loop_count == 0)
            return DHT_ERROR_TIMEOUT;
    }
    DHT_SDA_OUTPUT();
    DHT_SDA_HIGH();
    return DHT_OK;
}

uint8_t dht_read(void) {
    uint8_t result = _dht_read();
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
