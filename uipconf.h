#ifndef __UIPCONF_H__
#define __UIPCONF_H__

#include <inttypes.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdbool.h>

/**
 * Statistics datatype
 *
 * This typedef defines the dataype used for keeping statistics in
 * uIP.
 *
 * \hideinitializer
 */
typedef unsigned short uip_stats_t;

/**
 * Maximum number of TCP connections.
 *
 * \hideinitializer
 */
#define UIP_CONF_MAX_CONNECTIONS 1

/**
 * Maximum number of listening TCP ports.
 *
 * \hideinitializer
 */
#define UIP_CONF_MAX_LISTENPORTS 1

/**
 * uIP buffer size.
 *
 * \hideinitializer
 */
#define UIP_CONF_BUFFER_SIZE     600
/**
 * CPU byte order.
 *
 * \hideinitializer
 */
#define UIP_CONF_BYTE_ORDER      LITTLE_ENDIAN

/**
 * Logging on or off
 *
 * \hideinitializer
 */
#define UIP_CONF_LOGGING         0

/**
 * UDP support on or off requires DHCP if ON
 *
 * \hideinitializer
 */
#define UIP_CONF_UDP            0

/**
 * UDP checksums on or off
 *
 * \hideinitializer
 */
#define UIP_CONF_UDP_CHECKSUMS  0

/**
 * uIP statistics on or off
 *
 * \hideinitializer
 */
#define UIP_CONF_STATISTICS     1

/**
 * Broadcast support. Needed for dhcp
 *
 * \hideinitializer
 */
#define UIP_CONF_BROADCAST      0

/**
 * The maximum amount of concurrent UDP connections.
 *
 * \hideinitializer
 */
#define UIP_CONF_UDP_CONNS      0

/**
 *  Turn on IP packet re-assembly.
 *  This will re-assemble ip packets that become fragmented
 *    which when assembled will fit in your UIP BUFFER.
 *
 *  Note: This will double the amount of RAM used by the UIP BUFFER.
 */
#define UIP_REASSEMBLY 0

#endif /* __UIP_CONF_H__ */

/** @} */
/** @} */
