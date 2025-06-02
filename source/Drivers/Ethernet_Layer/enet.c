/*
 * enet.c
 *
 *  Created on: 1 jun. 2025
 *      Author: kenne
 */

#include <Drivers/Ethernet_Layer/enet.h>
#include "fsl_enet.h"
#include "fsl_phy.h"
#include "fsl_enet_mdio.h"
#include "fsl_phyksz8081.h"
#include "fsl_sysmpu.h"
#include "pin_mux.h"
#include "fsl_port.h"

#define PHY_ADDRESS 0x00U
/* MDIO operations. */
#define MDIO_OPS enet_ops
/* PHY operations. */
#define PHY_OPS phyksz8081_ops
/* ENET clock frequency. */
#define ENET_CLOCK_FREQ CLOCK_GetFreq(kCLOCK_CoreSysClk)
#define ENET_RXBD_NUM          (4)
#define ENET_TXBD_NUM          (4)
#define ENET_RXBUFF_SIZE       (ENET_FRAME_MAX_FRAMELEN)
#define ENET_TXBUFF_SIZE       (ENET_FRAME_MAX_FRAMELEN)
#define ENET_DATA_LENGTH       (60)
#define APP_ENET_BUFF_ALIGNMENT ENET_BUFF_ALIGNMENT
#define PHY_AUTONEGO_TIMEOUT_COUNT (100000)

#define PHY_STABILITY_DELAY_US (0U)

/*! @brief Buffer descriptors should be in non-cacheable region and should be align to "ENET_BUFF_ALIGNMENT". */
AT_NONCACHEABLE_SECTION_ALIGN(enet_rx_bd_struct_t g_rxBuffDescrip[ENET_RXBD_NUM], ENET_BUFF_ALIGNMENT);
AT_NONCACHEABLE_SECTION_ALIGN(enet_tx_bd_struct_t g_txBuffDescrip[ENET_TXBD_NUM], ENET_BUFF_ALIGNMENT);

SDK_ALIGN(uint8_t g_rxDataBuff[ENET_RXBD_NUM][SDK_SIZEALIGN(ENET_RXBUFF_SIZE, APP_ENET_BUFF_ALIGNMENT)],
          APP_ENET_BUFF_ALIGNMENT);
SDK_ALIGN(uint8_t g_txDataBuff[ENET_TXBD_NUM][SDK_SIZEALIGN(ENET_TXBUFF_SIZE, APP_ENET_BUFF_ALIGNMENT)],
          APP_ENET_BUFF_ALIGNMENT);

enet_handle_t g_handle;
uint8_t g_frame[ENET_DATA_LENGTH + 14];

/*! @brief Enet PHY and MDIO interface handler. */
static mdio_handle_t mdioHandle = {.ops = &MDIO_OPS};
static phy_handle_t phyHandle   = {.phyAddr = PHY_ADDRESS, .mdioHandle = &mdioHandle, .ops = &PHY_OPS};

static status_t status;
phy_config_t phyConfig = {0};
phy_speed_t speed;
phy_duplex_t duplex;
bool link = false;
enet_data_error_stats_t eErrStatic;
uint32_t length = 0;

static void ENET_ConfigPins(void){
	/* Port A Clock Gate Control: Clock enabled */
	CLOCK_EnableClock(kCLOCK_PortA);
	/* Port B Clock Gate Control: Clock enabled */
	CLOCK_EnableClock(kCLOCK_PortB);
	/* Port E Clock Gate Control: Clock enabled */
	CLOCK_EnableClock(kCLOCK_PortE);

	/* PORTA12 (pin K9) is configured as MII0_RXD1 */
	PORT_SetPinMux(PORTA, 12U, kPORT_MuxAlt4);

	/* PORTA13 (pin J9) is configured as MII0_RXD0 */
	PORT_SetPinMux(PORTA, 13U, kPORT_MuxAlt4);

	/* PORTA14 (pin L10) is configured as MII0_RXDV */
	PORT_SetPinMux(PORTA, 14U, kPORT_MuxAlt4);

	/* PORTA15 (pin L11) is configured as MII0_TXEN */
	PORT_SetPinMux(PORTA, 15U, kPORT_MuxAlt4);

	/* PORTA16 (pin K10) is configured as MII0_TXD0 */
	PORT_SetPinMux(PORTA, 16U, kPORT_MuxAlt4);

	/* PORTA17 (pin K11) is configured as MII0_TXD1 */
	PORT_SetPinMux(PORTA, 17U, kPORT_MuxAlt4);

	/* PORTA5 (pin M8) is configured as MII0_RXER */
	PORT_SetPinMux(PORTA, 5U, kPORT_MuxAlt4);

	const port_pin_config_t portb0_pinH10_config = {/* Internal pull-up/down resistor is disabled */
													kPORT_PullDisable,
													/* Fast slew rate is configured */
													kPORT_FastSlewRate,
													/* Passive filter is disabled */
													kPORT_PassiveFilterDisable,
													/* Open drain is enabled */
													kPORT_OpenDrainEnable,
													/* Low drive strength is configured */
													kPORT_LowDriveStrength,
													/* Pin is configured as MII0_MDIO */
													kPORT_MuxAlt4,
													/* Pin Control Register fields [15:0] are not locked */
													kPORT_UnlockRegister};
	/* PORTB0 (pin H10) is configured as MII0_MDIO */
	PORT_SetPinConfig(PORTB, 0U, &portb0_pinH10_config);

	/* PORTB1 (pin H9) is configured as MII0_MDC */
	PORT_SetPinMux(PORTB, 1U, kPORT_MuxAlt4);

	/* PORTE26 (pin K4) is configured as ENET_1588_CLKIN */
	PORT_SetPinMux(PORTE, 26U, kPORT_MuxAlt2);

	SIM->SOPT5 = ((SIM->SOPT5 &
				   /* Mask bits to zero which are setting */
				   (~(SIM_SOPT5_UART0TXSRC_MASK)))

				  /* UART 0 transmit data source select: UART0_TX pin. */
				  | SIM_SOPT5_UART0TXSRC(SOPT5_UART0TXSRC_UART_TX));

}

static void ENET_ConfigPHYLink(void){
	bool autonego          = false;
    volatile uint32_t count = 0;

	do
	{
		status = PHY_Init(&phyHandle, &phyConfig);
		if (status == kStatus_Success)
		{
			/* Wait for auto-negotiation success and link up */
			count = PHY_AUTONEGO_TIMEOUT_COUNT;
			do
			{
				PHY_GetAutoNegotiationStatus(&phyHandle, &autonego);
				PHY_GetLinkStatus(&phyHandle, &link);
				if (autonego && link)
				{
					break;
				}
			} while (--count);
			if (!autonego)
			{
				status = kStatus_Fail;
			}
		}
	} while (!(link && autonego));
}

static void ENET_BuildBroadCastFrame(uint8_t* g_ownMAC_Addr, uint8_t* g_pcMAC_Addr, uint8_t* g_data, size_t dataSize){
    uint32_t length = ENET_DATA_LENGTH - 14;

    memcpy(&g_frame[0], g_pcMAC_Addr, 6U);
    memcpy(&g_frame[6], g_ownMAC_Addr, 6U);

    g_frame[12] = (length >> 8) & 0xFFU;
    g_frame[13] = length & 0xFFU;

    memcpy(&g_frame[14], g_data, dataSize);
}

status_t ENET_Config(uint8_t* g_ownMAC_Addr){
	enet_config_t config;

	/* Hardware Initialization. */
	ENET_ConfigPins();
	/* Disable SYSMPU. */
	SYSMPU_Enable(SYSMPU, false);
	/* Set RMII clock src. */
	CLOCK_SetRmii0Clock(1U);

	/* Prepare the buffer configuration. */
	enet_buffer_config_t buffConfig[] = {{
		ENET_RXBD_NUM,
		ENET_TXBD_NUM,
		SDK_SIZEALIGN(ENET_RXBUFF_SIZE, APP_ENET_BUFF_ALIGNMENT),
		SDK_SIZEALIGN(ENET_TXBUFF_SIZE, APP_ENET_BUFF_ALIGNMENT),
		&g_rxBuffDescrip[0],
		&g_txBuffDescrip[0],
		&g_rxDataBuff[0][0],
		&g_txDataBuff[0][0],
		true,
		true,
		NULL,
	}};

	/* Get default configuration. */
	/*
	 * config.miiMode = kENET_RmiiMode;
	 * config.miiSpeed = kENET_MiiSpeed100M;
	 * config.miiDuplex = kENET_MiiFullDuplex;
	 * config.rxMaxFrameLen = ENET_FRAME_MAX_FRAMELEN;
	 */
	ENET_GetDefaultConfig(&config);

	config.miiMode = kENET_RmiiMode;

	phyConfig.phyAddr               = PHY_ADDRESS;
	phyConfig.autoNeg               = true;
	mdioHandle.resource.base        = ENET;
	mdioHandle.resource.csrClock_Hz = ENET_CLOCK_FREQ;

	ENET_ConfigPHYLink();
	if(status == kStatus_Success){
		/* Get the actual PHY link speed. */
		PHY_GetLinkSpeedDuplex(&phyHandle, &speed, &duplex);
		/* Change the MII speed and duplex for actual link status. */
		config.miiSpeed  = (enet_mii_speed_t)speed;
		config.miiDuplex = (enet_mii_duplex_t)duplex;

		ENET_Init(ENET, &g_handle, &config, &buffConfig[0], g_ownMAC_Addr, ENET_CLOCK_FREQ);
		ENET_ActiveRead(ENET);
	}
	else{
		status = kStatus_Fail;
	}
	return status;
}

status_t ENET_Tx(uint8_t* g_ownMAC_Addr, uint8_t* g_pcMAC_Addr, uint8_t* g_data, size_t dataSize){
	ENET_BuildBroadCastFrame(g_ownMAC_Addr, g_pcMAC_Addr, g_data, dataSize);

	/* Send a frame when the PHY is link up. */
	if (kStatus_Success == PHY_GetLinkStatus(&phyHandle, &link))
	{
		if (link)
		{
			if (kStatus_Success == ENET_SendFrame(ENET, &g_handle, &g_frame[0], ENET_DATA_LENGTH, 0, false, NULL))
			{
				status = kStatus_Success;
			}
			else
			{
				status = kStatus_Fail;
			}
		}
	}

	return status;
}

status_t ENET_Rx(uint8_t* g_ownMAC_Addr, uint8_t* g_pcMAC_Addr, uint8_t* g_rxBuffer, size_t bufferSize){
	/* Get the Frame size */
	status = ENET_GetRxFrameSize(&g_handle, &length, 0);
	/* Call ENET_ReadFrame when there is a received frame. */
	if (length != 0)
	{
		/* Received valid frame. Deliver the rx buffer with the size equal to length. */
		uint8_t *data = (uint8_t *)malloc(length);
		status        = ENET_ReadFrame(ENET, &g_handle, data, length, 0, NULL);
		if (status == kStatus_Success)
		{
			memcpy(g_rxBuffer, data, length);
		}
		free(data);
	}
	else if (status == kStatus_ENET_RxFrameError)
	{
		/* Update the received buffer when error happened. */
		/* Get the error information of the received g_frame. */
		ENET_GetRxErrBeforeReadFrame(&g_handle, &eErrStatic, 0);
		/* update the receive buffer. */
		ENET_ReadFrame(ENET, &g_handle, NULL, 0, 0, NULL);
	}
	return status;
}
