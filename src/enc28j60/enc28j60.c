/*! \file enc28j60.c \brief Microchip ENC28J60 Ethernet Interface Driver. */
//*****************************************************************************
//
// File Name    : 'enc28j60.c'
// Title        : Microchip ENC28J60 Ethernet Interface Driver
// Author       : Pascal Stang (c)2005
// Created      : 9/22/2005
// Revised      : 9/22/2005
// Version      : 0.1
// Target MCU   : Atmel AVR series
// Editor Tabs  : 4
//
// Description  : This driver provides initialization and transmit/receive
//  functions for the Microchip ENC28J60 10Mb Ethernet Controller and PHY.
// This chip is novel in that it is a full MAC+PHY interface all in a 28-pin
// chip, using an SPI interface to the host processor.
//
//*****************************************************************************

#include <avr/io.h>
#include <util/delay.h>
#include "../common.h"
#include "../config.h"
#include "enc28j60.h"

#define enc28j60_assert_cs() \
    ENC28J60_CONTROL_PORT &= ~(_BV(ENC28J60_CONTROL_CS))

#define enc28j60_release_cs() \
    ENC28J60_CONTROL_PORT |= _BV(ENC28J60_CONTROL_CS)

#define enc28j60_loop_spi_transmission_complete() \
    while(!(SPSR & _BV(SPIF)))

static uint8_t enc28j60_bank;
static uint16_t enc28j60_packet_ptr;

uint8_t enc28j60_op_read(uint8_t op, uint8_t address) {
    uint8_t data;
    enc28j60_assert_cs();
    /* Issue read command. */
    SPDR = op | (address & ADDR_MASK);
    enc28j60_loop_spi_transmission_complete();
    /* Read data. */
    SPDR = 0x00;
    enc28j60_loop_spi_transmission_complete();
    /* Do dummy read if needed. */
    if (address & 0x80) {
        SPDR = 0x00;
        enc28j60_loop_spi_transmission_complete();
    }
    data = SPDR;
    enc28j60_release_cs();
    return data;
}

void enc28j60_op_write(uint8_t op, uint8_t address, uint8_t data) {
    enc28j60_assert_cs();
    /* Issue write command. */
    SPDR = op | (address & ADDR_MASK);
    enc28j60_loop_spi_transmission_complete();
    /* Write data. */
    SPDR = data;
    enc28j60_loop_spi_transmission_complete();
    enc28j60_release_cs();
}

void enc28j60_buffer_read(uint16_t len, uint8_t *data) {
    enc28j60_assert_cs();
    /* Issue read command. */
    SPDR = ENC28J60_READ_BUF_MEM;
    enc28j60_loop_spi_transmission_complete();
    while (len--) {
        /* Read data. */
        SPDR = 0x00;
        enc28j60_loop_spi_transmission_complete();
        *data++ = SPDR;
    }
    enc28j60_release_cs();
}

void enc28j60_buffer_write(uint16_t len, uint8_t *data) {
    enc28j60_assert_cs();
    /* Issue write command. */
    SPDR = ENC28J60_WRITE_BUF_MEM;
    enc28j60_loop_spi_transmission_complete();
    while (len--) {
        /* Write data. */
        SPDR = *data++;
        enc28j60_loop_spi_transmission_complete();
    }
    enc28j60_release_cs();
}

void enc28j60_bank_set(uint8_t address) {
    /* Set the bank (if needed). */
    if ((address & BANK_MASK) != enc28j60_bank) {
        /* Set the bank. */
        enc28j60_op_write(ENC28J60_BIT_FIELD_CLR, ECON1, (ECON1_BSEL1 | ECON1_BSEL0));
        enc28j60_op_write(ENC28J60_BIT_FIELD_SET, ECON1, (address & BANK_MASK) >> 5);
        enc28j60_bank = (address & BANK_MASK);
    }
}

uint8_t enc28j60_read(uint8_t address) {
    /* Set the bank. */
    enc28j60_bank_set(address);
    /* Do the read. */
    return enc28j60_op_read(ENC28J60_READ_CTRL_REG, address);
}

void enc28j60_write(uint8_t address, uint8_t data) {
    /* Set the bank. */
    enc28j60_bank_set(address);
    /* Do the write. */
    enc28j60_op_write(ENC28J60_WRITE_CTRL_REG, address, data);
}

uint16_t enc28j60_phy_read(uint8_t address) {
    uint16_t data;
    /* Set the right address and start the register read operation. */
    enc28j60_write(MIREGADR, address);
    enc28j60_write(MICMD, MICMD_MIIRD);
    /* Wait until the PHY read completes. */
    while (enc28j60_read(MISTAT) & MISTAT_BUSY);
    /* Quit reading. */
    enc28j60_write(MICMD, 0x00);
    /* Get data value. */
    data  = enc28j60_read(MIRDL);
    data |= enc28j60_read(MIRDH);
    /* Return the data. */
    return data;
}

void enc28j60_phy_write(uint8_t address, uint16_t data) {
    /* Set the PHY register address. */
    enc28j60_write(MIREGADR, address);
    /* Write the PHY data. */
    enc28j60_write(MIWRL, data);
    enc28j60_write(MIWRH, data >> 8);
    /* Wait until the PHY write completes. */
    while (enc28j60_read(MISTAT) & MISTAT_BUSY);
}

void enc28j60_init(void) {
    enc28j60_spi_init();
    /* Perform system reset. */
    enc28j60_op_write(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
    /* Check CLKRDY bit to see if reset is complete. */
    _delay_us(50);
    while (!(enc28j60_read(ESTAT) & ESTAT_CLKRDY));
    /*
     * Do bank 0 stuff.
     * Initialize receive buffer.
     * 16-bit transfers, must write low byte first.
     * Set receive buffer start address.
     */
    enc28j60_packet_ptr = RXSTART_INIT;
    enc28j60_write(ERXSTL, RXSTART_INIT & 0xFF);
    enc28j60_write(ERXSTH, RXSTART_INIT >> 8);
    /* set receive pointer address. */
    enc28j60_write(ERXRDPTL, RXSTART_INIT & 0xFF);
    enc28j60_write(ERXRDPTH, RXSTART_INIT >> 8);
    /* Set receive buffer end. ERXND defaults to 0x1FFF (end of ram). */
    enc28j60_write(ERXNDL, RXSTOP_INIT & 0xFF);
    enc28j60_write(ERXNDH, RXSTOP_INIT >> 8);
    /* Set transmit buffer start. ETXST defaults to 0x0000 (beginnging of ram). */
    enc28j60_write(ETXSTL, TXSTART_INIT & 0xFF);
    enc28j60_write(ETXSTH, TXSTART_INIT >> 8);
    /* Do bank 2 stuff. Enable MAC receive. */
    enc28j60_write(MACON1, MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS);
    /* Bring MAC out of reset. */
    enc28j60_write(MACON2, 0x00);
    /* Enable automatic padding and CRC operations. */
    enc28j60_op_write(ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN);
    /* Set inter-frame gap (non-back-to-back). */
    enc28j60_write(MAIPGL, 0x12);
    enc28j60_write(MAIPGH, 0x0C);
    /* Set inter-frame gap (back-to-back). */
    enc28j60_write(MABBIPG, 0x12);
    /* Set the maximum packet size which the controller will accept. */
    enc28j60_write(MAMXFLL, MAX_FRAMELEN & 0xFF);
    enc28j60_write(MAMXFLH, MAX_FRAMELEN >> 8);
    enc28j60_set_mac();
    /* No loopback of transmitted frames. */
    enc28j60_phy_write(PHCON2, PHCON2_HDLDIS);
    /* Switch to bank 0. */
    enc28j60_bank_set(ECON1);
    /* Enable interrutps. */
    enc28j60_op_write(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE | EIE_PKTIE);
    /* Enable packet reception. */
    enc28j60_op_write(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
}

void enc28j60_spi_init(void) {
    /* Initialize I/O. */
    ENC28J60_CONTROL_DDR |= _BV(ENC28J60_CONTROL_CS);
    ENC28J60_CONTROL_PORT |= _BV(ENC28J60_CONTROL_CS);
    /* etup SPI I/O pins. */
    /* Set SCK low. */
    ENC28J60_SPI_PORT &= ~(_BV(ENC28J60_SPI_SCK));
    /* Set SCK as output, MOSI as output, SS must be output for Master mode to work. */
    ENC28J60_SPI_DDR |= (_BV(ENC28J60_SPI_SCK) | _BV(ENC28J60_SPI_MOSI) | _BV(ENC28J60_SPI_SS));
    /* Set MISO as input. */
    ENC28J60_SPI_DDR &= ~(_BV(ENC28J60_SPI_MISO));
    /* Enable SPI, master mode. */
    SPCR = _BV(SPE) | _BV(MSTR);
}

void enc28j60_set_mac(void) {
    /*
     * Do bank 3 stuff.
     * Write MAC address.
     * NOTE: MAC address in ENC28J60 is byte-backward.
     */
    enc28j60_write(MAADR5, ETH_ADDR0);
    enc28j60_write(MAADR4, ETH_ADDR1);
    enc28j60_write(MAADR3, ETH_ADDR2);
    enc28j60_write(MAADR2, ETH_ADDR3);
    enc28j60_write(MAADR1, ETH_ADDR4);
    enc28j60_write(MAADR0, ETH_ADDR5);
}

void enc28j60_packet_send(uint16_t len1, uint8_t *packet1, uint16_t len2, uint8_t *packet2) {
    enc28j60_op_write(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRST);
    enc28j60_op_write(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRST);
    /* Set the write pointer to start of transmit buffer area. */
    enc28j60_write(EWRPTL, TXSTART_INIT & 0xff);
    enc28j60_write(EWRPTH, TXSTART_INIT >> 8);
    /* Set the TXND pointer to correspond to the packet size given. */
    enc28j60_write(ETXNDL, (TXSTART_INIT + len1 + len2));
    enc28j60_write(ETXNDH, (TXSTART_INIT + len1 + len2) >> 8);
    /* Write per-packet control byte. */
    enc28j60_op_write(ENC28J60_WRITE_BUF_MEM, 0, 0x00);
    /* Copy the packet into the transmit buffer. */
    enc28j60_buffer_write(len1, packet1);
    if (len2 > 0)
        enc28j60_buffer_write(len2, packet2);
    /* Send the contents of the transmit buffer onto the network. */
    enc28j60_op_write(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
}

uint16_t enc28j60_packet_receive(uint16_t maxlen, uint8_t *packet) {
    uint16_t rxstat;
    uint16_t len;
    /* Check if a packet has been received and buffered. */
    if (!enc28j60_read(EPKTCNT))
        return 0;
    /* Make absolutely certain that any previous packet was discarded. */
    /* Set the read pointer to the start of the received packet. */
    enc28j60_write(ERDPTL, (enc28j60_packet_ptr));
    enc28j60_write(ERDPTH, (enc28j60_packet_ptr) >> 8);
    /* Read the next packet pointer. */
    enc28j60_packet_ptr  = enc28j60_op_read(ENC28J60_READ_BUF_MEM, 0);
    enc28j60_packet_ptr |= enc28j60_op_read(ENC28J60_READ_BUF_MEM, 0) << 8;
    /* Read the packet length. */
    len  = enc28j60_op_read(ENC28J60_READ_BUF_MEM, 0);
    len |= enc28j60_op_read(ENC28J60_READ_BUF_MEM, 0) << 8;
    /* Read the receive status. */
    rxstat  = enc28j60_op_read(ENC28J60_READ_BUF_MEM, 0);
    rxstat |= enc28j60_op_read(ENC28J60_READ_BUF_MEM, 0) << 8;
    /* Limit retrieve length (we reduce the MAC-reported length by 4 to remove the CRC). */
    len = min(len, maxlen);
    /* Copy the packet from the receive buffer. */
    enc28j60_buffer_read(len, packet);
    /*
     * Move the RX read pointer to the start of the next received packet. This frees
     * the memory we just read out.
     */
    enc28j60_write(ERXRDPTL, (enc28j60_packet_ptr));
    enc28j60_write(ERXRDPTH, (enc28j60_packet_ptr) >> 8);
    /* Decrement the packet counter indicate we are done with this packet. */
    enc28j60_op_write(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
    return len;
}
