/*
 * Ethernet.h
 *
 *  Created on: 1 jun. 2025
 *      Author: kenne
 */

#ifndef DRIVERS_ETHERNET_H_
#define DRIVERS_ETHERNET_H_

#include "Ethernet_cfg.h"
#include "enet.h"

void Ethernet_Init(void);

status_t Ethernet_TX(uint8_t* buffer);

status_t Ethernet_RX(uint8_t* buffer);

#endif /* DRIVERS_ETHERNET_H_ */
