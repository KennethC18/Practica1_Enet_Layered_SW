#include <stdlib.h>
#include "pin_mux.h"
#include "board.h"
#include "fsl_debug_console.h"

#include "Drivers/Security_Layer/Security.h"
#include "Drivers/Ethernet_Layer/Ethernet.h"

#define MSG1  "No todo lo que es oro reluce..."
#define MSG2  "Aún en la oscuridad..."
#define MSG3  "¿Qué es la vida?"
#define MSG4  "No temas a la oscuridad..."
#define MSG5  "Hasta los más pequeños..."
#define MSG6  "No digas que el sol se ha puesto..."
#define MSG7  "El coraje se encuentra..."
#define MSG8  "No todos los tesoros..."
#define MSG9  "Es peligroso..."
#define MSG10 "Un mago nunca llega tarde..."
#define MSG11 "Aún hay esperanza..."
#define MSG12 "El mundo está cambiando..."
#define MSG13 "Las raíces profundas..."
#define MSG14 "No se puede..."
#define MSG15 "Y sobre todo..."
#define MSG16 "De las cenizas, un fuego..."

#define RSP1  "...Ni todos los que vagan están perdidos."
#define RSP2  "...brilla una luz."
#define RSP3  "Nada más que un breve caminar a la luz del sol."
#define RSP4  "...pues en ella se esconden las estrellas."
#define RSP5  "...pueden cambiar el curso del futuro."
#define RSP6  "...si aún te queda la luna."
#define RSP7  "...en los lugares más inesperados."
#define RSP8  "...son oro y plata."
#define RSP9  "...cruzar tu puerta."
#define RSP10 "...ni pronto, Frodo Bolsón. Llega precisamente cuando se lo propone."
#define RSP11 "...mientras la Compañía permanezca fiel."
#define RSP12 "...Siento que algo se avecina."
#define RSP13 "...no alcanzan las heladas."
#define RSP14 "...pasar."
#define RSP15 "...cuidado con el Anillo."
#define RSP16 "...se despertará."

#define MAX_BUFFER_SIZE 80

char* messages[] = {
    MSG1,
    MSG2,
    MSG3,
    MSG4,
    MSG5,
    MSG6,
    MSG7,
    MSG8,
    MSG9,
    MSG10,
    MSG11,
    MSG12,
    MSG13,
    MSG14,
    MSG15,
    MSG16
};

char* responses[] = {
    RSP1,
    RSP2,
    RSP3,
    RSP4,
    RSP5,
    RSP6,
    RSP7,
    RSP8,
    RSP9,
    RSP10,
    RSP11,
    RSP12,
    RSP13,
    RSP14,
    RSP15,
    RSP16
};

uint8_t enc_tx_msg[MAX_BUFFER_SIZE] = {0};
uint8_t buffer_TX[MAX_BUFFER_SIZE] = {0};

uint8_t buffer_RX[MAX_BUFFER_SIZE] = {0};
uint8_t enc_rx_msg[MAX_BUFFER_SIZE] = {0};
uint8_t dec_rx_msg[MAX_BUFFER_SIZE] = {0};

status_t valid_packet = kStatus_Fail;
bool valid_chksum = false;

void PRINT_BufferString(uint8_t* buffer){
	uint8_t i = 0;
	while(buffer[i] != 0){
		PRINTF("%c",buffer[i]);
		i++;
	}
	PRINTF("\r\n");
}

void PRINT_BufferHex(uint8_t* buffer){
	uint8_t i = 0;
	while(i < MAX_BUFFER_SIZE){
		if(buffer[i] <= 0xF){
			PRINTF("0");
		}
		PRINTF("%X,",buffer[i]);
		i++;
	}
	PRINTF("\r\n");
}

int main (void){
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitDebugConsole();

	Security_init();
	Ethernet_Init();

	uint8_t i = 0;
	while(1){
		Security_Encrypt((uint8_t*)messages[i], enc_tx_msg);
		Security_AddChecksum(enc_tx_msg, buffer_TX);
		Ethernet_TX(buffer_TX);

		do {
			valid_packet = Ethernet_RX(buffer_RX);
		} while (valid_packet != kStatus_Success);

		valid_chksum = Security_ValidChecksum(buffer_RX, enc_rx_msg);
		if(valid_chksum){
			Security_Decrypt(enc_rx_msg, dec_rx_msg);
		}
		else{
			PRINTF("Checksum failed\r\n");
		}

		PRINTF("Message TX Buffer : ");
		PRINT_BufferString((uint8_t*)messages[i]);
		PRINTF("Encoded TX Buffer : ");
		PRINT_BufferHex(enc_tx_msg);
		PRINTF("Ethernet TX Buffer: ");
		PRINT_BufferHex(buffer_TX);

		PRINTF("Ethernet RX Buffer: ");
		PRINT_BufferHex(buffer_RX);
		PRINTF("Encoded RX Buffer : ");
		PRINT_BufferHex(enc_rx_msg);
		PRINTF("Decoded RXBuffer  : ");
		PRINT_BufferString(dec_rx_msg);

		memset(enc_tx_msg, 0x00, MAX_BUFFER_SIZE);
		memset(buffer_TX, 0x00, MAX_BUFFER_SIZE);
		memset(enc_rx_msg, 0x00, MAX_BUFFER_SIZE);
		memset(buffer_RX, 0x00, MAX_BUFFER_SIZE);
		memset(dec_rx_msg, 0x00, MAX_BUFFER_SIZE);

		i++;
	}
}
