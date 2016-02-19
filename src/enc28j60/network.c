#include <avr/io.h>
#include <util/delay.h>
#include "../uip/uip.h"
#include "enc28j60.h"
#include "network.h"

inline uint16_t network_read(void) {
    return enc28j60_packet_receive(UIP_BUFSIZE, uip_buf);
}

void network_send(void) {
    if (uip_len <= UIP_LLH_LEN + 40)
        enc28j60_packet_send(uip_len, uip_buf, 0, 0);
    else
        enc28j60_packet_send(54, uip_buf , uip_len - UIP_LLH_LEN - 40, uip_appdata);
}

void network_init(void) {
    /* Initialize the device. */
    enc28j60_init();

    /* Configure leds. */
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

