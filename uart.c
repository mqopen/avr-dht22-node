#include <avr/io.h>
//#include <avr/interrupt.h>
// #include <avr/pgmspace.h>
#include "uart.h"

#define U2X_BITMASK 0x8000

void uart_init(uint16_t baudrate) {
    UCSR0A |= _BV(U2X0);
    
    uint16_t baud_setting = F_CPU / 8 / baudrate - 1;
    
    UBRR0H = (unsigned char) (baud_setting >> 8);
    UBRR0L = (unsigned char) baud_setting;
    
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00) | _BV(USBS0); /* 8-bit data */
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */
}

void uart_putc(unsigned char data) {
    UDR0 = data;
    loop_until_bit_is_set(UCSR0A, UDRE0);
}

void uart_puts(const char *s) {
    while (*s) 
        uart_putc(*s++);
}

void uart_println(const char *s) {
    uart_puts(s);
    uart_puts("\r\n");
}
