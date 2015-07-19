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

/* Static function prototypes. */
static void _interface_init(void);
#if !(CONFIG_DHCP)
static void _ip_init();
#endif

void tcpip_output(void) {
}

int main (void) {
    uart_init(BAUD);
    clock_init();
    dht_init();
    network_init();
    uip_init();
    node_init();
    _interface_init();
#if !(CONFIG_DHCP)
    _ip_init();
#endif
    
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

static void _interface_init(void) {
    struct uip_eth_addr mac;
    
    mac.addr[0] = ETH_ADDR0;
    mac.addr[1] = ETH_ADDR1;
    mac.addr[2] = ETH_ADDR2;
    mac.addr[3] = ETH_ADDR3;
    mac.addr[4] = ETH_ADDR4;
    mac.addr[5] = ETH_ADDR5;

    uip_setethaddr(mac);
    
    timer_set(&periodic_timer, CLOCK_SECOND / 2);
    timer_set(&arp_timer, CLOCK_SECOND * 10);
}

#if !(CONFIG_DHCP)
static void _ip_init() {
    uip_ipaddr_t address;
    uip_ipaddr_t netmask;
    
    uip_ipaddr(&address, CONFIG_IP_ADDR0, CONFIG_IP_ADDR1, CONFIG_IP_ADDR2, CONFIG_IP_ADDR3);
    uip_ipaddr(&netmask, CONFIG_NETMASK0, CONFIG_NETMASK1, CONFIG_NETMASK2, CONFIG_NETMASK3);
    
    uip_sethostaddr(&address);
    uip_setnetmask(&netmask);
}
#endif
