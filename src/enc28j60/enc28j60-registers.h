/*
 * Copyright (C) Ivo Slanina <ivo.slanina@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __ENC28J60_REGISTERS_H__
#define __ENC28J60_REGISTERS_H__

#define ADDR_MASK   0x1F
#define BANK_MASK   0x60
#define SPRD_MASK   0x80
// All-bank registers
#define EIE         0x1B
#define EIR         0x1C
#define ESTAT       0x1D
#define ECON2       0x1E
#define ECON1       0x1F
// Bank 0 registers
#define ERDPTL      (0x00|0x00)
#define ERDPTH      (0x01|0x00)
#define EWRPTL      (0x02|0x00)
#define EWRPTH      (0x03|0x00)
#define ETXSTL      (0x04|0x00)
#define ETXSTH      (0x05|0x00)
#define ETXNDL      (0x06|0x00)
#define ETXNDH      (0x07|0x00)
#define ERXSTL      (0x08|0x00)
#define ERXSTH      (0x09|0x00)
#define ERXNDL      (0x0A|0x00)
#define ERXNDH      (0x0B|0x00)
#define ERXRDPTL    (0x0C|0x00)
#define ERXRDPTH    (0x0D|0x00)
#define ERXWRPTL    (0x0E|0x00)
#define ERXWRPTH    (0x0F|0x00)
#define EDMASTL     (0x10|0x00)
#define EDMASTH     (0x11|0x00)
#define EDMANDL     (0x12|0x00)
#define EDMANDH     (0x13|0x00)
#define EDMADSTL    (0x14|0x00)
#define EDMADSTH    (0x15|0x00)
#define EDMACSL     (0x16|0x00)
#define EDMACSH     (0x17|0x00)
// Bank 1 registers
#define EHT0        (0x00|0x20)
#define EHT1        (0x01|0x20)
#define EHT2        (0x02|0x20)
#define EHT3        (0x03|0x20)
#define EHT4        (0x04|0x20)
#define EHT5        (0x05|0x20)
#define EHT6        (0x06|0x20)
#define EHT7        (0x07|0x20)
#define EPMM0       (0x08|0x20)
#define EPMM1       (0x09|0x20)
#define EPMM2       (0x0A|0x20)
#define EPMM3       (0x0B|0x20)
#define EPMM4       (0x0C|0x20)
#define EPMM5       (0x0D|0x20)
#define EPMM6       (0x0E|0x20)
#define EPMM7       (0x0F|0x20)
#define EPMCSL      (0x10|0x20)
#define EPMCSH      (0x11|0x20)
#define EPMOL       (0x14|0x20)
#define EPMOH       (0x15|0x20)
#define EWOLIE      (0x16|0x20)
#define EWOLIR      (0x17|0x20)
#define ERXFCON     (0x18|0x20)
#define EPKTCNT     (0x19|0x20)
// Bank 2 registers
#define MACON1      (0x00|0x40|0x80)
#define MACON2      (0x01|0x40|0x80)
#define MACON3      (0x02|0x40|0x80)
#define MACON4      (0x03|0x40|0x80)
#define MABBIPG     (0x04|0x40|0x80)
#define MAIPGL      (0x06|0x40|0x80)
#define MAIPGH      (0x07|0x40|0x80)
#define MACLCON1    (0x08|0x40|0x80)
#define MACLCON2    (0x09|0x40|0x80)
#define MAMXFLL     (0x0A|0x40|0x80)
#define MAMXFLH     (0x0B|0x40|0x80)
#define MAPHSUP     (0x0D|0x40|0x80)
#define MICON       (0x11|0x40|0x80)
#define MICMD       (0x12|0x40|0x80)
#define MIREGADR    (0x14|0x40|0x80)
#define MIWRL       (0x16|0x40|0x80)
#define MIWRH       (0x17|0x40|0x80)
#define MIRDL       (0x18|0x40|0x80)
#define MIRDH       (0x19|0x40|0x80)
// Bank 3 registers
#define MAADR1      (0x00|0x60|0x80)
#define MAADR0      (0x01|0x60|0x80)
#define MAADR3      (0x02|0x60|0x80)
#define MAADR2      (0x03|0x60|0x80)
#define MAADR5      (0x04|0x60|0x80)
#define MAADR4      (0x05|0x60|0x80)
#define EBSTSD      (0x06|0x60)
#define EBSTCON     (0x07|0x60)
#define EBSTCSL     (0x08|0x60)
#define EBSTCSH     (0x09|0x60)
#define MISTAT      (0x0A|0x60|0x80)
#define EREVID      (0x12|0x60)
#define ECOCON      (0x15|0x60)
#define EFLOCON     (0x17|0x60)
#define EPAUSL      (0x18|0x60)
#define EPAUSH      (0x19|0x60)
// PHY registers
#define PHCON1      0x00
#define PHSTAT1     0x01
#define PHHID1      0x02
#define PHHID2      0x03
#define PHCON2      0x10
#define PHSTAT2     0x11
#define PHIE        0x12
#define PHIR        0x13
#define PHLCON      0x14
#endif