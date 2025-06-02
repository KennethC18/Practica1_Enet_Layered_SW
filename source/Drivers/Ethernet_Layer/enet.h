/*
 * enet.h
 *
 *  Created on: 1 jun. 2025
 *      Author: kenne
 */

#ifndef DRIVERS_ENET_H_
#define DRIVERS_ENET_H_

#include "fsl_common.h"

status_t ENET_Config(uint8_t* g_ownMAC_Addr);

status_t ENET_Tx(uint8_t* g_ownMAC_Addr, uint8_t* g_pcMAC_Addr, uint8_t* g_data, size_t dataSize);

status_t ENET_Rx(uint8_t* g_ownMAC_Addr, uint8_t* g_pcMAC_Addr, uint8_t* g_rxBuffer, size_t bufferSize);

#endif /* DRIVERS_ENET_H_ */
