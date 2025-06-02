/*
 * Security.c
 *
 *  Created on: 30 May 2025
 *      Author: kenneth
 */
#include "Security.h"

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

void Security_MakeChecksum(uint8_t* buffer, uint32_t* checksum){
	*checksum = CRC32_GetChecksum(buffer, BUFFER_SIZE);
}

