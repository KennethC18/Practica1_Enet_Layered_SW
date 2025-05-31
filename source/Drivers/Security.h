/*
 * Security.h
 *
 *  Created on: 30 May 2025
 *      Author: kenneth
 */

#ifndef DRIVERS_SECURITY_H_
#define DRIVERS_SECURITY_H_

#include "aes.h"
#include "Security_cfg.h"
#include "string.h"

#define BUFFER_SIZE 32

void Security_init();

void Security_send(uint8_t* in_buffer, uint8_t* out_buffer);

void Security_receive(uint8_t* in_buffer, uint8_t* out_buffer);


#endif /* DRIVERS_SECURITY_H_ */
