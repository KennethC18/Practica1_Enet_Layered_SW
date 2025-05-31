/*
 * Security.c
 *
 *  Created on: 30 May 2025
 *      Author: kenneth
 */
#include "Security.h"

struct AES_ctx ctx;

void Security_init(){
	AES_init_ctx_iv(&ctx, AES128_KEY, AES128_IV);
}

void Security_send(uint8_t* in_buffer, uint8_t* out_buffer){
	memcpy(out_buffer, in_buffer, BUFFER_SIZE);

	AES_ctx_set_iv(&ctx, AES128_IV);

	AES_CBC_encrypt_buffer(&ctx, out_buffer, BUFFER_SIZE);
}

void Security_receive(uint8_t* in_buffer, uint8_t* out_buffer){
	memcpy(out_buffer, in_buffer, BUFFER_SIZE);

	AES_ctx_set_iv(&ctx, AES128_IV);

	AES_CBC_decrypt_buffer(&ctx, out_buffer, BUFFER_SIZE);
}

