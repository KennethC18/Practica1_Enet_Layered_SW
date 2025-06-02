#include <stdlib.h>
#include "pin_mux.h"
#include "board.h"
#include "fsl_debug_console.h"

#include "Drivers/Security_Layer/Security.h"
#include "Drivers/Ethernet_Layer/Ethernet.h"

uint8_t msg1[32] = "No todo lo que es oro reluce...";

uint8_t enc_msg[32];

uint32_t checksum;

uint8_t buffer_TX[36];

uint8_t buffer_RX[36];

uint8_t enc_rx_msg[32];

uint8_t dec_rx_msg[32];

status_t st = kStatus_Fail;

bool v_chksum = false;

int main (void){
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitDebugConsole();

	Security_init();
	Ethernet_Init();

	Security_Encrypt(msg1, enc_msg);

	Security_AddChecksum(enc_msg, buffer_TX);

	Ethernet_TX(buffer_TX);

	while(1){
		st = Ethernet_RX(buffer_RX);
		if(st == kStatus_Success){
			v_chksum = Security_ValidChecksum(buffer_RX, enc_rx_msg);
			if(!v_chksum){
				Security_Decrypt(enc_rx_msg, dec_rx_msg);
				PRINTF("%s\r\n",dec_rx_msg);
			}
			else{
				PRINTF("checksum failed\r\n");
			}
		}
		else{
//			PRINTF("rx packet failed\r\n");
		}
	}
}
