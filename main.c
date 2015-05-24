#define BAUD 19200

#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
#include "enc28j60/network.h"
#include "enc28j60/enc28j60.h"

void tcpip_output(void) {
}

int main (void) {
    uart_init(BAUD);
    network_init();
    
    while(1)  {
        uart_puts("test\r\n");
        enc28j60_phy_write(PHLCON, PHLCON_LACFG_ON | PHLCON_LBCFG_ON | PHLCON_LFRQ_TMSTRCH | PHLCON_STRCH);
        _delay_ms(1000);
        enc28j60_phy_write(PHLCON, PHLCON_LACFG_OFF | PHLCON_LBCFG_OFF | PHLCON_LFRQ_TMSTRCH | PHLCON_STRCH);
        _delay_ms(1000);
    }
}
