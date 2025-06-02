/*
 * Security.h
 *
 *  Created on: 30 May 2025
 *      Author: kenneth
 */

#ifndef DRIVERS_SECURITY_H_
#define DRIVERS_SECURITY_H_

#include "aes.h"
#include "crc.h"
#include "Security_cfg.h"
#include "string.h"

void Security_init();

void Security_Encrypt(uint8_t* in_buffer, uint8_t* out_buffer);

void Security_Decrypt(uint8_t* in_buffer, uint8_t* out_buffer);

void Security_MakeChecksum();

#endif /* DRIVERS_SECURITY_H_ */
