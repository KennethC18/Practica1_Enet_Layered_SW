/*
 * crc.c
 *
 *  Created on: 1 jun. 2025
 *      Author: kenne
 */

#include <Drivers/Security_Layer/crc.h>

/*!
 * @brief      Init for CRC-32.
 * @details    Init CRC peripheral module for CRC-32 protocol.
 *             width=32 poly=0x04c11db7 init=0xffffffff refin=true refout=true xorout=0xffffffff
 * @parameters seed
 */
void CRC32_Init(uint32_t seed){


	crc_config_t config;

	config.polynomial         = 0x04C11DB7U;
	config.seed               = seed;
	config.reflectIn          = true;
	config.reflectOut         = true;
	config.complementChecksum = true;
	config.crcBits            = kCrcBits32;
	config.crcResult          = kCrcFinalChecksum;

	CRC_Init(CRC0, &config);
}

uint32_t CRC32_GetChecksum(uint32_t seed, uint8_t *data, size_t dataSize){
	CRC32_Init(seed);
    CRC_WriteData(CRC0, (uint8_t *)&data[0], dataSize);
    return CRC_Get32bitResult(CRC0);
}
