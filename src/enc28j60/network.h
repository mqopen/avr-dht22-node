#ifndef __NETWORK_H__
#define __NETWORK_H__

/**
 * Initialize the network
 */
void network_init(void);

/**
 * Initialize the network with a mac addr
 *
 * @param macaddr
 */
void network_init_mac(uint8_t *macaddr);

/**
 * Read from the network, returns number of read bytes
 *
 * @return
 */
inline uint16_t network_read(void);

/**
 * Send using the network
 */
void network_send(void);

/**
 * Sets the MAC address of the device
 *
 * @param mac
 */
void network_set_MAC(uint8_t *mac);

/**
 * Gets the MAC address of the device
 *
 * @param mac
 */
void network_get_MAC(uint8_t *mac);

/**
 * Get the state of the network link on the interface 1 up, 0 down.
 *
 * @return
 */
uint8_t network_link_state(void);

#endif
