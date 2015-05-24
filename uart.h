#ifndef __UART_H__
#define __UART_H__

void uart_init(uint16_t baudrate);

void uart_putc(unsigned char data);

void uart_puts(const char *s );

void uart_println(const char *s );
#endif // __UART_H__
