/*
 * crc.h
 *
 *  Created on: 1 jun. 2025
 *      Author: kenne
 */

#ifndef CRC_H_
#define CRC_H_

#include "fsl_crc.h"

void CRC32_Init(uint32_t seed);

uint32_t CRC32_GetChecksum(uint8_t* data, size_t dataSize);

#endif /* CRC_H_ */
