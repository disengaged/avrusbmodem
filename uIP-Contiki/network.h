/*
 * Simple common network interface that all network drivers should implement.
 */

#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <avr/io.h>
#include <util/delay.h>
#include "USBModem.h"
#include "modem.h"
#include "uart.h"
#include "uip-fw.h"

/*Initialize the network*/
void network_init(void);

/*Read from the network, returns number of read bytes*/
unsigned int network_read(void);

/*Send using the network*/
void network_send(void);

/*Sets the MAC address of the device*/
//void network_set_MAC(uint8_t* mac);

/*Gets the MAC address of the device*/
//void network_get_MAC(uint8_t* mac);

#endif /* __NETWORK_H__ */
