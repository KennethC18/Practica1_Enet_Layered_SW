/*
 * Ethernet.c
 *
 *  Created on: 1 jun. 2025
 *      Author: kenne
 */
#include "Ethernet.h"

uint8_t ownMAC[6] = OWN_MAC_ADDRESS;
uint8_t pcMAC[6] = PC_MAC_ADDRESS;

void Ethernet_Init(void){
	ENET_Config(ownMAC);
}

status_t Ethernet_TX(uint8_t* buffer){
	status_t status = kStatus_Fail;

	status = ENET_Tx(ownMAC, pcMAC, buffer, DATA_SIZE);

	return status;
}

status_t Ethernet_RX(uint8_t* buffer){
	status_t status = kStatus_Fail;

	status = ENET_Rx(ownMAC, pcMAC, buffer, DATA_SIZE);

	return status;
}
