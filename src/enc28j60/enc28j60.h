/*! \file enc28j60.h \brief Microchip ENC28J60 Ethernet Interface Driver. */
//*****************************************************************************
//
// File Name    : 'enc28j60.h'
// Title        : Microchip ENC28J60 Ethernet Interface Driver
// Author       : Pascal Stang (c)2005
// Created      : 9/22/2005
// Revised      : 9/22/2005
// Version      : 0.1
// Target MCU   : Atmel AVR series
// Editor Tabs  : 4
//
/// \ingroup network
/// \defgroup enc28j60 Microchip ENC28J60 Ethernet Interface Driver (enc28j60.c)
/// \code #include "net/enc28j60.h" \endcode
/// \par Overview
///     This driver provides initialization and transmit/receive
/// functions for the Microchip ENC28J60 10Mb Ethernet Controller and PHY.
/// This chip is novel in that it is a full MAC+PHY interface all in a 28-pin
/// chip, using an SPI interface to the host processor.
///
//
//*****************************************************************************
//@{

#ifndef __ENC28J60_H__
#define __ENC28J60_H__

#include "enc28j60-registers.h"

// ENC28J60 EIE Register Bit Definitions
#define EIE_INTIE       0x80
#define EIE_PKTIE       0x40
#define EIE_DMAIE       0x20
#define EIE_LINKIE      0x10
#define EIE_TXIE        0x08
#define EIE_WOLIE       0x04
#define EIE_TXERIE      0x02
#define EIE_RXERIE      0x01
// ENC28J60 EIR Register Bit Definitions
#define EIR_PKTIF       0x40
#define EIR_DMAIF       0x20
#define EIR_LINKIF      0x10
#define EIR_TXIF        0x08
#define EIR_WOLIF       0x04
#define EIR_TXERIF      0x02
#define EIR_RXERIF      0x01
// ENC28J60 ESTAT Register Bit Definitions
#define ESTAT_INT       0x80
#define ESTAT_LATECOL   0x10
#define ESTAT_RXBUSY    0x04
#define ESTAT_TXABRT    0x02
#define ESTAT_CLKRDY    0x01
// ENC28J60 ECON2 Register Bit Definitions
#define ECON2_AUTOINC   0x80
#define ECON2_PKTDEC    0x40
#define ECON2_PWRSV     0x20
#define ECON2_VRPS      0x08
// ENC28J60 ECON1 Register Bit Definitions
#define ECON1_TXRST     0x80
#define ECON1_RXRST     0x40
#define ECON1_DMAST     0x20
#define ECON1_CSUMEN    0x10
#define ECON1_TXRTS     0x08
#define ECON1_RXEN      0x04
#define ECON1_BSEL1     0x02
#define ECON1_BSEL0     0x01
// ENC28J60 MACON1 Register Bit Definitions
#define MACON1_LOOPBK   0x10
#define MACON1_TXPAUS   0x08
#define MACON1_RXPAUS   0x04
#define MACON1_PASSALL  0x02
#define MACON1_MARXEN   0x01
// ENC28J60 MACON2 Register Bit Definitions
#define MACON2_MARST    0x80
#define MACON2_RNDRST   0x40
#define MACON2_MARXRST  0x08
#define MACON2_RFUNRST  0x04
#define MACON2_MATXRST  0x02
#define MACON2_TFUNRST  0x01
// ENC28J60 MACON3 Register Bit Definitions
#define MACON3_PADCFG2  0x80
#define MACON3_PADCFG1  0x40
#define MACON3_PADCFG0  0x20
#define MACON3_TXCRCEN  0x10
#define MACON3_PHDRLEN  0x08
#define MACON3_HFRMLEN  0x04
#define MACON3_FRMLNEN  0x02
#define MACON3_FULDPX   0x01
// ENC28J60 MICMD Register Bit Definitions
#define MICMD_MIISCAN   0x02
#define MICMD_MIIRD     0x01
// ENC28J60 MISTAT Register Bit Definitions
#define MISTAT_NVALID   0x04
#define MISTAT_SCAN     0x02
#define MISTAT_BUSY     0x01
// ENC28J60 PHY PHCON1 Register Bit Definitions
#define PHCON1_PRST     0x8000
#define PHCON1_PLOOPBK  0x4000
#define PHCON1_PPWRSV   0x0800
#define PHCON1_PDPXMD   0x0100
// ENC28J60 PHY PHSTAT1 Register Bit Definitions
#define PHSTAT1_PFDPX   0x1000
#define PHSTAT1_PHDPX   0x0800
#define PHSTAT1_LLSTAT  0x0004
#define PHSTAT1_JBSTAT  0x0002
// ENC28J60 PHY PHCON2 Register Bit Definitions
#define PHCON2_FRCLINK  0x4000
#define PHCON2_TXDIS    0x2000
#define PHCON2_JABBER   0x0400
#define PHCON2_HDLDIS   0x0100

// ENC28J60 Packet Control Byte Bit Definitions
#define PKTCTRL_PHUGEEN     0x08
#define PKTCTRL_PPADEN      0x04
#define PKTCTRL_PCRCEN      0x02
#define PKTCTRL_POVERRIDE   0x01

// SPI operation codes
#define ENC28J60_READ_CTRL_REG  0x00
#define ENC28J60_READ_BUF_MEM   0x3A
#define ENC28J60_WRITE_CTRL_REG 0x40
#define ENC28J60_WRITE_BUF_MEM  0x7A
#define ENC28J60_BIT_FIELD_SET  0x80
#define ENC28J60_BIT_FIELD_CLR  0xA0
#define ENC28J60_SOFT_RESET     0xFF

// buffer boundaries applied to internal 8K ram
//  entire available packet buffer space is allocated
#define TXSTART_INIT    0x0000  // start TX buffer at 0
#define RXSTART_INIT    0x0600  // give TX buffer space for one full ethernet frame (~1500 bytes)
#define RXSTOP_INIT     0x1FFF  // receive buffer gets the rest

#define MAX_FRAMELEN    1518    // maximum ethernet frame length

// Ethernet constants
#define ETHERNET_MIN_PACKET_LENGTH  0x3C

#define PHLCON_LxCFG_TRANSMIT_ACTIVITY                      0x1
#define PHLCON_LxCFG_RECEIVE_ACTIVITY                       0x2
#define PHLCON_LxCFG_COLLISION_ACTIVITY                     0x3
#define PHLCON_LxCFG_LINK_STATUS                            0x4
#define PHLCON_LxCFG_DUPLEX_STATUS                          0x5
#define PHLCON_LxCFG_TRANSMIT_RECEIVE_ACTIVITY              0x7
#define PHLCON_LxCFG_ON                                     0x8
#define PHLCON_LxCFG_OFF                                    0x9
#define PHLCON_LxCFG_BLINK_FAST                             0xa
#define PHLCON_LxCFG_BLINK_SLOW                             0xb
#define PHLCON_LxCFG_LINK_STATUS_RECEIVE_ACTIVITY           0xc
#define PHLCON_LxCFG_LINK_STATUS_TRANSMIT_RECEIVE_ACTIVITY  0xd
#define PHLCON_LxCFG_DUPLEX_STATUS_COLISION_ACTIVITY        0xe
// PHLCON shifts
#define PHLCON_STRCH_SHIFT      1
#define PHLCON_LFRQ_SHIFT       2
#define PHLCON_LBCFG_SHIFT      4
#define PHLCON_LACFG_SHIFT      8
// PHLCON values
// PHLCON STRCH values
#define PHLCON_STRCH                                        (0x01 << PHLCON_STRCH_SHIFT)
// PHLCON LFRQ values
#define PHLCON_LFRQ_TNSTRCH                                 (0x00 << PHLCON_LFRQ_SHIFT)
#define PHLCON_LFRQ_TMSTRCH                                 (0x01 << PHLCON_LFRQ_SHIFT)
#define PHLCON_LFRQ_TLSTRCH                                 (0x02 << PHLCON_LFRQ_SHIFT)
// PHLCON LBCFG values
#define PHLCON_LBCFG_TRANSMIT_ACTIVITY                      (PHLCON_LxCFG_TRANSMIT_ACTIVITY << PHLCON_LBCFG_SHIFT)
#define PHLCON_LBCFG_RECEIVE_ACTIVITY                       (PHLCON_LxCFG_RECEIVE_ACTIVITY << PHLCON_LBCFG_SHIFT)
#define PHLCON_LBCFG_COLLISION_ACTIVITY                     (PHLCON_LxCFG_COLLISION_ACTIVITY << PHLCON_LBCFG_SHIFT)
#define PHLCON_LBCFG_LINK_STATUS                            (PHLCON_LxCFG_LINK_STATUS << PHLCON_LBCFG_SHIFT)
#define PHLCON_LBCFG_DUPLEX_STATUS                          (PHLCON_LxCFG_DUPLEX_STATUS << PHLCON_LBCFG_SHIFT)
#define PHLCON_LBCFG_TRANSMIT_RECEIVE_ACTIVITY              (PHLCON_LxCFG_TRANSMIT_RECEIVE_ACTIVITY << PHLCON_LBCFG_SHIFT)
#define PHLCON_LBCFG_ON                                     (PHLCON_LxCFG_ON << PHLCON_LBCFG_SHIFT)
#define PHLCON_LBCFG_OFF                                    (PHLCON_LxCFG_OFF << PHLCON_LBCFG_SHIFT)
#define PHLCON_LBCFG_BLINK_FAST                             (PHLCON_LxCFG_BLINK_FAST << PHLCON_LBCFG_SHIFT)
#define PHLCON_LBCFG_BLINK_SLOW                             (PHLCON_LxCFG_BLINK_SLOW << PHLCON_LBCFG_SHIFT)
#define PHLCON_LBCFG_LINK_STATUS_RECEIVE_ACTIVITY           (PHLCON_LxCFG_LINK_STATUS_RECEIVE_ACTIVITY << PHLCON_LBCFG_SHIFT)
#define PHLCON_LBCFG_LINK_STATUS_TRANSMIT_RECEIVE_ACTIVITY  (PHLCON_LxCFG_LINK_STATUS_TRANSMIT_RECEIVE_ACTIVITY << PHLCON_LBCFG_SHIFT)
#define PHLCON_LBCFG_DUPLEX_STATUS_COLISION_ACTIVITY        (PHLCON_LxCFG_DUPLEX_STATUS_COLISION_ACTIVITY << PHLCON_LBCFG_SHIFT)
// PHLCON LACFG values
#define PHLCON_LACFG_TRANSMIT_ACTIVITY                      (PHLCON_LxCFG_TRANSMIT_ACTIVITY << PHLCON_LACFG_SHIFT)
#define PHLCON_LACFG_RECEIVE_ACTIVITY                       (PHLCON_LxCFG_RECEIVE_ACTIVITY << PHLCON_LACFG_SHIFT)
#define PHLCON_LACFG_COLLISION_ACTIVITY                     (PHLCON_LxCFG_COLLISION_ACTIVITY << PHLCON_LACFG_SHIFT)
#define PHLCON_LACFG_LINK_STATUS                            (PHLCON_LxCFG_LINK_STATUS << PHLCON_LACFG_SHIFT)
#define PHLCON_LACFG_DUPLEX_STATUS                          (PHLCON_LxCFG_DUPLEX_STATUS << PHLCON_LACFG_SHIFT)
#define PHLCON_LACFG_TRANSMIT_RECEIVE_ACTIVITY              (PHLCON_LxCFG_TRANSMIT_RECEIVE_ACTIVITY << PHLCON_LACFG_SHIFT)
#define PHLCON_LACFG_ON                                     (PHLCON_LxCFG_ON << PHLCON_LACFG_SHIFT)
#define PHLCON_LACFG_OFF                                    (PHLCON_LxCFG_OFF << PHLCON_LACFG_SHIFT)
#define PHLCON_LACFG_BLINK_FAST                             (PHLCON_LxCFG_BLINK_FAST << PHLCON_LACFG_SHIFT)
#define PHLCON_LACFG_BLINK_SLOW                             (PHLCON_LxCFG_BLINK_SLOW << PHLCON_LACFG_SHIFT)
#define PHLCON_LACFG_LINK_STATUS_RECEIVE_ACTIVITY           (PHLCON_LxCFG_LINK_STATUS_RECEIVE_ACTIVITY << PHLCON_LACFG_SHIFT)
#define PHLCON_LACFG_LINK_STATUS_TRANSMIT_RECEIVE_ACTIVITY  (PHLCON_LxCFG_LINK_STATUS_TRANSMIT_RECEIVE_ACTIVITY << PHLCON_LACFG_SHIFT)
#define PHLCON_LACFG_DUPLEX_STATUS_COLISION_ACTIVITY        (PHLCON_LxCFG_DUPLEX_STATUS_COLISION_ACTIVITY << PHLCON_LACFG_SHIFT)

//! do a ENC28J60 read operation
uint8_t enc28j60_op_read(uint8_t op, uint8_t address);

//! do a ENC28J60 write operation
void enc28j60_op_write(uint8_t op, uint8_t address, uint8_t data);

//! read the packet buffer memory
void enc28j60_buffer_read(uint16_t len, uint8_t *data);

//! write the packet buffer memory
void enc28j60_buffer_write(uint16_t len, uint8_t *data);

//! set the register bank for register at address
void enc28j60_bank_set(uint8_t address);

//! read ax88796 register
uint8_t enc28j60_read(uint8_t address);

//! write ax88796 register
void enc28j60_write(uint8_t address, uint8_t data);

//! read a PHY register
uint16_t enc28j60_phy_read(uint8_t address);

//! write a PHY register
void enc28j60_phy_write(uint8_t address, uint16_t data);

//! initialize the ethernet interface for transmit/receive
void enc28j60_init(void);

void enc28j60_spi_init(void);

void enc28j60_set_mac(void);

//! Packet transmit function.
/// Sends a packet on the network.  It is assumed that the packet is headed by a valid ethernet header.
/// \param len      Length of packet in bytes.
/// \param packet   Pointer to packet data.
/// \param len2     Length of the secound packet in bytes, can be 0.
/// \param packet2  Pointer to the secound packet data, can be NULL.
void enc28j60_packet_send(uint16_t len1, uint8_t *packet1, uint16_t len2, uint8_t *packet2);

//! Packet receive function.
/// Gets a packet from the network receive buffer, if one is available.
/// The packet will by headed by an ethernet header.
/// \param  maxlen  The maximum acceptable length of a retrieved packet.
/// \param  packet  Pointer where packet data should be stored.
/// \return Packet length in bytes if a packet was retrieved, zero otherwise.
uint16_t enc28j60_packet_receive(uint16_t maxlen, uint8_t *packet);

#endif
//@}
