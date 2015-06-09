#define BAUD 19200

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "enc28j60/network.h"
#include "enc28j60/enc28j60.h"
#include "uip/uip.h"
#include "uip/uiparp.h"
#include "uip/timer.h"
#include "uip/clock.h"
#include "nethandler.h"
#include "dht.h"
#include "umqtt/umqtt.h"

#include "uart.h"

static volatile bool flag_packet_rx = true;

void tcpip_output(void) {
}

static uint8_t mqtt_txbuff[200];
static uint8_t mqtt_rxbuff[150];

static void handle_message(struct umqtt_connection __attribute__((unused)) *conn, char *topic, uint8_t *data, int len) {
}

static struct umqtt_connection mqtt = {
    .txbuff = {
        .start = mqtt_txbuff,
        .length = sizeof(mqtt_txbuff),
    },
    .rxbuff = {
        .start = mqtt_rxbuff,
        .length = sizeof(mqtt_rxbuff),
    },
    .message_callback = handle_message,
};

int main (void) {
    uart_init(BAUD);
    clock_init();
    dht_init();
    network_init();
    uip_init();

    EIMSK = _BV(INT0);
    EICRA = _BV(ISC01);
    enc28j60_write(EIE, 0b11000000);

    sei();

    struct uip_eth_addr mac;
    uip_ipaddr_t ip;
    uip_ipaddr_t netmask;

    mac.addr[0] = ETH_ADDR0;
    mac.addr[1] = ETH_ADDR1;
    mac.addr[2] = ETH_ADDR2;
    mac.addr[3] = ETH_ADDR3;
    mac.addr[4] = ETH_ADDR4;
    mac.addr[5] = ETH_ADDR5;

    uip_setethaddr(mac);
    uip_ipaddr(&ip, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    uip_ipaddr(&netmask, IP_NETMASK0, IP_NETMASK1, IP_NETMASK2, IP_NETMASK3);
    uip_sethostaddr(&ip);
    uip_setnetmask(&netmask);

    struct timer periodic_timer;
    struct timer arp_timer;
    struct timer hello_timer;
    timer_set(&periodic_timer, CLOCK_SECOND / 2);
    timer_set(&arp_timer, CLOCK_SECOND * 10);
    timer_set(&hello_timer, CLOCK_SECOND);

    nethandler_umqtt_init(&mqtt);
    
    for (;;) {
        if (flag_packet_rx) {
            flag_packet_rx = false;
            nethandler_rx();
        }

        if (timer_tryrestart(&periodic_timer))
            nethandler_periodic();

        if (timer_tryrestart(&arp_timer))
            uip_arp_timer();
            
        //if (timer_tryrestart(&hello_timer))
        //    umqtt_publish(&mqtt, "test", (uint8_t *)"hello", 5);
    }
    return 0;
}

ISR(INT0_vect) {
    flag_packet_rx = true;
}
