/*
 * Security.c
 *
 *  Created on: 30 May 2025
 *      Author: kenneth
 */
#include <Drivers/Security_Layer/Security.h>

struct AES_ctx ctx;

static size_t GetBufferSize(uint8_t* buffer) {
	size_t count = 0;

	while (count + 2 < 80) {
		if (buffer[count] == 0x00 && buffer[count + 1] == 0x00 && buffer[count + 2] == 0x00) {
			break;
		}
		else{
			count++;
		}
	}

	return count;
}

static size_t GetArraySize(uint8_t* buffer){
	size_t count = 0;

	while (buffer[count] != 0) {
		count++;
	}

	return count+1;
}

static void BufferAddPadding(uint8_t* buffer) {
    size_t bufSize = GetBufferSize(buffer);

    size_t padding_len = AES_BLOCKLEN - (bufSize % AES_BLOCKLEN);

    // For PKCS#5/PKCS#7, always add padding (even if bufSize is a multiple of 16)
    // If bufSize % 16 == 0, add a full block (16 bytes, each 0x10)
    if (padding_len == 0) {
        padding_len = AES_BLOCKLEN;
    }

    memset(buffer + bufSize, (uint8_t)padding_len, padding_len);
}

static void BufferRemovePadding(uint8_t* buffer) {
	size_t bufSize = GetBufferSize(buffer);

	size_t padding_len = buffer[bufSize - 1];

	memset(&buffer[bufSize - padding_len], 0x00, padding_len);
}

void Security_init(){
	AES_init_ctx_iv(&ctx, (uint8_t *)AES128_KEY, (uint8_t *)AES128_IV);
	CRC32_Init(CRC32_SEED);
}

void Security_Encrypt(uint8_t* in_buffer, uint8_t* out_buffer){
    size_t bufSize = GetArraySize(in_buffer);

	memcpy(out_buffer, in_buffer, bufSize);

	BufferAddPadding(out_buffer);

	AES_ctx_set_iv(&ctx, (uint8_t *)AES128_IV);

	AES_CBC_encrypt_buffer(&ctx, out_buffer, bufSize);
}

void Security_Decrypt(uint8_t* in_buffer, uint8_t* out_buffer){
    size_t bufSize = GetBufferSize(in_buffer);

	memcpy(out_buffer, in_buffer, bufSize);

	AES_ctx_set_iv(&ctx, (uint8_t *)AES128_IV);

	AES_CBC_decrypt_buffer(&ctx, out_buffer, bufSize);

	BufferRemovePadding(out_buffer);
}

void Security_AddChecksum(uint8_t* in_buffer, uint8_t* out_buffer){
    size_t bufSize = GetBufferSize(in_buffer);

	uint32_t checksum = 0;

	memcpy(out_buffer, in_buffer, bufSize);

	checksum = CRC32_GetChecksum(CRC32_SEED, in_buffer, bufSize);

	for (uint8_t i = bufSize; i < (bufSize + 4); ++i) {
		out_buffer[i] = (uint8_t)(checksum >> (8 * (i - bufSize)) & (0xFF));
	}
}

bool Security_ValidChecksum(uint8_t* in_buffer, uint8_t* out_buffer){
    size_t bufSize = GetBufferSize(in_buffer) - 4;

	bool retval = false;
	uint32_t rx_checksum = 0;
	uint32_t real_checksum = 0;

	memcpy(out_buffer, in_buffer, bufSize);

	real_checksum = CRC32_GetChecksum(CRC32_SEED, out_buffer, bufSize);

	for (uint8_t i = bufSize; i < (bufSize + 4); ++i) {
		rx_checksum |= (uint32_t)(in_buffer[i] << (8 * (i - bufSize)));
	}

	if (rx_checksum == real_checksum){
		retval = true;
	}
	else{
		retval = false;
	}

	return retval;
}
