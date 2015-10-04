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

#include <avr/io.h>
#include "uart.h"

#define U2X_BITMASK 0x8000

void uart_init(uint16_t baudrate) {
    UCSR0A |= _BV(U2X0);

    uint16_t baud_setting = F_CPU / 8 / baudrate - 1;

    UBRR0H = (uint8_t) (baud_setting >> 8);
    UBRR0L = (uint8_t) baud_setting;

    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00) | _BV(USBS0); /* 8-bit data */
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */
}

void uart_putc(uint8_t data) {
    UDR0 = data;
    loop_until_bit_is_set(UCSR0A, UDRE0);
}

void uart_puts(char *s) {
    while (*s)
        uart_putc(*s++);
}

void uart_println(char *s) {
    uart_puts(s);
    uart_puts("\r\n");
}
