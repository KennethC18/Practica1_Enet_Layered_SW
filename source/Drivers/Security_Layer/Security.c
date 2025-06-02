/*
 * Security.c
 *
 *  Created on: 30 May 2025
 *      Author: kenneth
 */
#include <Drivers/Security_Layer/Security.h>

struct AES_ctx ctx;

void Security_init(){
	AES_init_ctx_iv(&ctx, (uint8_t *)AES128_KEY, (uint8_t *)AES128_IV);
	CRC32_Init(CRC32_SEED);
}

void Security_Encrypt(uint8_t* in_buffer, uint8_t* out_buffer){
	memcpy(out_buffer, in_buffer, BUFFER_SIZE);

	AES_ctx_set_iv(&ctx, (uint8_t *)AES128_IV);

	AES_CBC_encrypt_buffer(&ctx, out_buffer, BUFFER_SIZE);
}

void Security_Decrypt(uint8_t* in_buffer, uint8_t* out_buffer){
	memcpy(out_buffer, in_buffer, BUFFER_SIZE);

	AES_ctx_set_iv(&ctx, (uint8_t *)AES128_IV);

	AES_CBC_decrypt_buffer(&ctx, out_buffer, BUFFER_SIZE);
}

void Security_AddChecksum(uint8_t* in_buffer, uint8_t* out_buffer){
	uint32_t checksum = 0;

	memcpy(out_buffer, in_buffer, BUFFER_SIZE);

	checksum = CRC32_GetChecksum(CRC32_SEED, in_buffer, BUFFER_SIZE);

	for (uint8_t i = BUFFER_SIZE; i < (BUFFER_SIZE + 4); ++i) {
		out_buffer[i] = (uint8_t)(checksum >> (8 * (i - BUFFER_SIZE)) & (0xFF));
	}
}

bool Security_ValidChecksum(uint8_t* in_buffer, uint8_t* out_buffer){
	bool retval = false;
	uint32_t rx_checksum = 0;
	uint32_t real_checksum = 0;

	memcpy(out_buffer, in_buffer, BUFFER_SIZE);

	real_checksum = CRC32_GetChecksum(CRC32_SEED, out_buffer, BUFFER_SIZE);

	for (uint8_t i = BUFFER_SIZE; i < (BUFFER_SIZE + 4); ++i) {
		rx_checksum |= (uint32_t)(in_buffer[i] << (8 * (i - BUFFER_SIZE)));
	}

	if (rx_checksum == real_checksum){
		retval = true;
	}
	else{
		retval = false;
	}

	return retval;
}
