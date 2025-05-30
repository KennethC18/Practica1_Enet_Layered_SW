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

void Security_init();

void Security_send();

void Security_receive();


#endif /* DRIVERS_SECURITY_H_ */
