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

#define DHT_TIMEOUT 400
#define DHT_INITIAL_MASK 128

/* Data from last measurement. */
struct dht_data dht_data;

/* Buffer to receive data. */
static uint8_t bits[5];

/* Translation unit private function prototypes */
uint8_t _dht_read(void);

void dht_init(void) {
    /* Configure DHT SDA pin as output. */
    DHT_DDR |= _BV(DHT_SDA);
    
    /* Set DHT SDA pin high. */
    DHT_PORT |= _BV(DHT_SDA);
}

uint8_t _dht_read(void) {
    uint8_t mask = DHT_INITIAL_MASK;
    uint8_t idx = 0;
    uint8_t data = 0;
    uint8_t state;
    uint8_t previous_state = 0;
    uint16_t zero_loop = DHT_TIMEOUT;
    uint16_t delta = 0;
    uint8_t i;

    /* Magic constant for now. */
    uint8_t leading_zero_bits = 36;
    
    /* Set DHT SDA pin as output. */
    DHT_DDR |= _BV(DHT_SDA);
    
    /* Set DHT SDA pin low. */
    DHT_PORT &= ~(_BV(DHT_SDA));
    
    /* Magic constant for now. */
    _delay_ms(1);
    
    /* Set DHT SDA pin high. */
    DHT_PORT &= _BV(DHT_SDA);
    
    /* Set DHT SDA pin as input. */
    DHT_DDR |= ~(_BV(DHT_SDA));
    
    uint16_t loop_count = DHT_TIMEOUT * 2;
    while(bit_is_set(DHT_PIN, DHT_SDA)) {
        if (--loop_count == 0)
            return DHT_ERROR_CONNECT;
    }
    
    loop_count = DHT_TIMEOUT;
    while(bit_is_clear(DHT_PIN, DHT_SDA)) {
        if (--loop_count == 0)
            return DHT_ERROR_ACK_L;
    }
    
    loop_count = DHT_TIMEOUT;
    while(bit_is_set(DHT_PIN, DHT_SDA)) {
        if (--loop_count == 0)
            return DHT_ERROR_ACK_H;
    }
    
    loop_count = DHT_TIMEOUT;
    for (i = 40; i != 0; ) {
        state = bit_is_set(DHT_PIN, DHT_SDA);
        if(state == 0 && previous_state != 0) {
            if(i > leading_zero_bits) {
                zero_loop = min(zero_loop, loop_count);
                delta = (DHT_TIMEOUT - zero_loop) / 4;
            } else if(loop_count <= (zero_loop - delta)) {
                data |= mask;
            }
            mask >>= 1;
            if(mask == 0) {
                mask = DHT_INITIAL_MASK;
                bits[idx] = data;
                idx++;
                data = 0;
            }
            --i;
            loop_count = DHT_TIMEOUT;
        }
        previous_state = state;
        if(--loop_count == 0)
            return DHT_ERROR_TIMEOUT;
    }
    
    /* Set DHT SDA pin as output. */
    DHT_DDR |= _BV(DHT_SDA);
    
    /* Set DHT SDA pin high. */
    DHT_PORT &= _BV(DHT_SDA);
    
    return DHT_OK;
}

uint8_t dht_read(void) {
    uint8_t result = _dht_read();
    bits[0] &= 0x03;
    bits[2] &= 0x83;
    
    // test checksum
    uint8_t sum = bits[0] + bits[1] + bits[2] + bits[3];
    if (bits[4] != sum) {
        return DHT_ERROR_CHECKSUM;
    }
    
    dht_data.humidity = ((bits[0] << 8) | bits[1]);
    dht_data.temperature = ((bits[2] << 8) | bits[3]);
    
    return result;
}
