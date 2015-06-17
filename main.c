#define BAUD 19200

#include <util/delay.h>
#include <avr/interrupt.h>
#include "enc28j60/network.h"
#include "uip/uip.h"
#include "uip/uiparp.h"
#include "uip/timer.h"
#include "nethandler.h"
#include "dht.h"
#include "node.h"
#include "uart.h"

static struct timer periodic_timer;
static struct timer arp_timer;

void tcpip_output(void) {
}

void interface_init() {
    struct uip_eth_addr mac;
    
    mac.addr[0] = ETH_ADDR0;
    mac.addr[1] = ETH_ADDR1;
    mac.addr[2] = ETH_ADDR2;
    mac.addr[3] = ETH_ADDR3;
    mac.addr[4] = ETH_ADDR4;
    mac.addr[5] = ETH_ADDR5;

    uip_setethaddr(mac);
    
    struct timer periodic_timer;
    struct timer arp_timer;
    timer_set(&periodic_timer, CLOCK_SECOND / 2);
    timer_set(&arp_timer, CLOCK_SECOND * 10);
}

int main (void) {
    uart_init(BAUD);
    clock_init();
    dht_init();
    network_init();
    uip_init();
    node_init();
    interface_init();
    
    sei();
    
    for (;;) {
        nethandler_rx();
        
        if (timer_tryrestart(&periodic_timer))
            nethandler_periodic();

        if (timer_tryrestart(&arp_timer))
            uip_arp_timer();
        
        node_process();
    }
    return 0;
}
