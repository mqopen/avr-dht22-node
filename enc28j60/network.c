#include <avr/io.h>
#include <util/delay.h>
#include "../uip/uip.h"
#include "enc28j60.h"
#include "network.h"

unsigned int network_read(void) {
    return ((uint16_t) enc28j60_packet_receive(UIP_BUFSIZE, (uint8_t *)uip_buf));
}

void network_send(void) {
    if (uip_len <= UIP_LLH_LEN + 40)
        enc28j60_packet_send(uip_len, (uint8_t *)uip_buf, 0, 0);
    else
        enc28j60_packet_send(54, (uint8_t *)uip_buf , uip_len - UIP_LLH_LEN - 40, (uint8_t *)uip_appdata);
}

void network_init(void) {
    //Initialise the device
    enc28j60_init();

    //Configure leds
    enc28j60_phy_write(PHLCON,
                        PHLCON_LACFG_LINK_STATUS |
                            PHLCON_LBCFG_TRANSMIT_RECEIVE_ACTIVITY |
                            PHLCON_LFRQ_TMSTRCH |
                            PHLCON_STRCH);
}

void network_get_MAC(uint8_t *macaddr) {
    // read MAC address registers
    // NOTE: MAC address in ENC28J60 is byte-backward
    *macaddr++ = enc28j60_read(MAADR5);
    *macaddr++ = enc28j60_read(MAADR4);
    *macaddr++ = enc28j60_read(MAADR3);
    *macaddr++ = enc28j60_read(MAADR2);
    *macaddr++ = enc28j60_read(MAADR1);
    *macaddr++ = enc28j60_read(MAADR0);
}

void network_set_MAC(uint8_t *macaddr) {
    // write MAC address
    // NOTE: MAC address in ENC28J60 is byte-backward
    enc28j60_write(MAADR5, *macaddr++);
    enc28j60_write(MAADR4, *macaddr++);
    enc28j60_write(MAADR3, *macaddr++);
    enc28j60_write(MAADR2, *macaddr++);
    enc28j60_write(MAADR1, *macaddr++);
    enc28j60_write(MAADR0, *macaddr++);
}

