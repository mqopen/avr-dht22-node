#define BAUD 19200

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "enc28j60/network.h"
#include "enc28j60/enc28j60.h"
#include "uip/uip.h"
#include "uip/uip-arp.h"
#include "uip/timer.h"
#include "nethandler.h"

void tcpip_output(void) {
}

static volatile bool flag_packet_rx = false;

int main (void) {
    uart_init(BAUD);
    network_init();
    uip_init();
    
    struct uip_eth_addr mac;
    uip_ipaddr_t ip;
    
    mac.addr[0] = ETHADDR0;
    mac.addr[1] = ETHADDR1;
    mac.addr[2] = ETHADDR2;
    mac.addr[3] = ETHADDR3;
    mac.addr[4] = ETHADDR4;
    mac.addr[5] = ETHADDR5;
    
    uip_setethaddr(mac);
    uip_ipaddr(&ip, 192, 168, 1, 201);
    uip_sethostaddr(&ip);
    
    struct uip_conn *uc;
    uip_ipaddr_t dst_ip;
    uip_ipaddr(&dst_ip, 192, 168, 1, 70);
    uc = uip_connect(&ip, htons(5555));
    
    if (uc == NULL) {
        /* check if connection was successful */
    }
    
    for(;;) {
        if(flag_packet_rx) {
            flag_packet_rx = false;
            nethandler_rx();
        }
        //uart_puts("test\r\n");
        //enc28j60_phy_write(PHLCON, PHLCON_LACFG_ON | PHLCON_LBCFG_ON | PHLCON_LFRQ_TMSTRCH | PHLCON_STRCH);
        //_delay_ms(1000);
        //enc28j60_phy_write(PHLCON, PHLCON_LACFG_OFF | PHLCON_LBCFG_OFF | PHLCON_LFRQ_TMSTRCH | PHLCON_STRCH);
        //_delay_ms(1000);
    }
    return 0;
}

ISR(INT0_vect) {
    flag_packet_rx = true;
}
