/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>
#include "pin_mux.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_enet.h"
#include "fsl_phy.h"
#if defined(FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET) && FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET
#include "fsl_memory.h"
#endif
#include "fsl_enet_mdio.h"
#include "fsl_phyksz8081.h"
#include "fsl_sysmpu.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* ENET base address */
#define EXAMPLE_ENET        ENET
#define EXAMPLE_PHY_ADDRESS 0x00U

/* MDIO operations. */
#define EXAMPLE_MDIO_OPS enet_ops
/* PHY operations. */
#define EXAMPLE_PHY_OPS phyksz8081_ops
/* ENET clock frequency. */
#define EXAMPLE_CLOCK_FREQ CLOCK_GetFreq(kCLOCK_CoreSysClk)
#define ENET_RXBD_NUM          (4)
#define ENET_TXBD_NUM          (4)
#define ENET_RXBUFF_SIZE       (ENET_FRAME_MAX_FRAMELEN)
#define ENET_TXBUFF_SIZE       (ENET_FRAME_MAX_FRAMELEN)
#define ENET_DATA_LENGTH       (60)
#define ENET_TRANSMIT_DATA_NUM (20)
#ifndef APP_ENET_BUFF_ALIGNMENT
#define APP_ENET_BUFF_ALIGNMENT ENET_BUFF_ALIGNMENT
#endif
#ifndef PHY_AUTONEGO_TIMEOUT_COUNT
#define PHY_AUTONEGO_TIMEOUT_COUNT (100000)
#endif
#ifndef PHY_STABILITY_DELAY_US
#define PHY_STABILITY_DELAY_US (0U)
#endif

/* @TEST_ANCHOR */

#ifndef MAC_ADDRESS
#define MAC_ADDRESS {0xd4, 0xbe, 0xd9, 0x45, 0x22, 0x60}
#endif

#define DEST_MAC_ADDR {0x10,0x62,0xe5,0xe1,0x01,0x06};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*! @brief Build ENET broadcast frame. */
static void ENET_BuildBroadCastFrame(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief Buffer descriptors should be in non-cacheable region and should be align to "ENET_BUFF_ALIGNMENT". */
AT_NONCACHEABLE_SECTION_ALIGN(enet_rx_bd_struct_t g_rxBuffDescrip[ENET_RXBD_NUM], ENET_BUFF_ALIGNMENT);
AT_NONCACHEABLE_SECTION_ALIGN(enet_tx_bd_struct_t g_txBuffDescrip[ENET_TXBD_NUM], ENET_BUFF_ALIGNMENT);
/*! @brief The data buffers can be in cacheable region or in non-cacheable region.
 * If use cacheable region, the alignment size should be the maximum size of "CACHE LINE SIZE" and "ENET_BUFF_ALIGNMENT"
 * If use non-cache region, the alignment size is the "ENET_BUFF_ALIGNMENT".
 */
SDK_ALIGN(uint8_t g_rxDataBuff[ENET_RXBD_NUM][SDK_SIZEALIGN(ENET_RXBUFF_SIZE, APP_ENET_BUFF_ALIGNMENT)],
          APP_ENET_BUFF_ALIGNMENT);
SDK_ALIGN(uint8_t g_txDataBuff[ENET_TXBD_NUM][SDK_SIZEALIGN(ENET_TXBUFF_SIZE, APP_ENET_BUFF_ALIGNMENT)],
          APP_ENET_BUFF_ALIGNMENT);

enet_handle_t g_handle;
uint8_t g_frame[ENET_DATA_LENGTH + 14];

/*! @brief The MAC address for ENET device. */
uint8_t g_macAddr[6] = MAC_ADDRESS;
uint8_t g_mac_destAddr[6] = DEST_MAC_ADDR;
uint8_t g_data_frame[] = "Kenneth Eduardo Castillo Garcia";

uint8_t g_rx_src_mac[6] = {0};


/*! @brief Enet PHY and MDIO interface handler. */
static mdio_handle_t mdioHandle = {.ops = &EXAMPLE_MDIO_OPS};
static phy_handle_t phyHandle   = {.phyAddr = EXAMPLE_PHY_ADDRESS, .mdioHandle = &mdioHandle, .ops = &EXAMPLE_PHY_OPS};

/*******************************************************************************
 * Code
 ******************************************************************************/
/*! @brief Build Frame for transmit. */
static void ENET_BuildBroadCastFrame(void)
{
    uint32_t count  = 0;
    uint32_t length = ENET_DATA_LENGTH - 14;

//    for (count = 0; count < 6U; count++)
//    {
//        g_frame[count] = 0xFFU;
//    }
    memcpy(&g_frame[0], &g_mac_destAddr[0], 6U);

    memcpy(&g_frame[6], &g_macAddr[0], 6U);
    g_frame[12] = (length >> 8) & 0xFFU;
    g_frame[13] = length & 0xFFU;

//    for (count = 0; count < length; count++)
//    {
//        g_frame[count + 14] = count % 0xFFU;
//    }

    memcpy(&g_frame[14], &g_data_frame[0], 32U);
}

/*!
 * @brief Main function
 */
int main(void)
{
    enet_config_t config;
    phy_config_t phyConfig = {0};
    uint32_t length        = 0;
    bool link              = false;
    bool autonego          = false;
    phy_speed_t speed;
    phy_duplex_t duplex;
    uint32_t testTxNum = 0;
    status_t status;
    enet_data_error_stats_t eErrStatic;
    volatile uint32_t count = 0;

    /* Hardware Initialization. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    /* Disable SYSMPU. */
    SYSMPU_Enable(SYSMPU, false);
    /* Set RMII clock src. */
    CLOCK_SetRmii0Clock(1U);

    PRINTF("\r\nENET example start.\r\n");

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

    /* The miiMode should be set according to the different PHY interfaces. */
#ifdef EXAMPLE_PHY_INTERFACE_RGMII
    config.miiMode = kENET_RgmiiMode;
#else
    config.miiMode = kENET_RmiiMode;
#endif
    phyConfig.phyAddr               = EXAMPLE_PHY_ADDRESS;
    phyConfig.autoNeg               = true;
    mdioHandle.resource.base        = EXAMPLE_ENET;
    mdioHandle.resource.csrClock_Hz = EXAMPLE_CLOCK_FREQ;

    /* Initialize PHY and wait auto-negotiation over. */
    PRINTF("Wait for PHY init...\r\n");
    do
    {
        status = PHY_Init(&phyHandle, &phyConfig);
        if (status == kStatus_Success)
        {
            PRINTF("Wait for PHY link up...\r\n");
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
                PRINTF("PHY Auto-negotiation failed. Please check the cable connection and link partner setting.\r\n");
            }
        }
    } while (!(link && autonego));

#if PHY_STABILITY_DELAY_US
    /* Wait a moment for PHY status to be stable. */
    SDK_DelayAtLeastUs(PHY_STABILITY_DELAY_US, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
#endif

    /* Get the actual PHY link speed. */
    PHY_GetLinkSpeedDuplex(&phyHandle, &speed, &duplex);
    /* Change the MII speed and duplex for actual link status. */
    config.miiSpeed  = (enet_mii_speed_t)speed;
    config.miiDuplex = (enet_mii_duplex_t)duplex;

    ENET_Init(EXAMPLE_ENET, &g_handle, &config, &buffConfig[0], &g_macAddr[0], EXAMPLE_CLOCK_FREQ);
    ENET_ActiveRead(EXAMPLE_ENET);

    /* Build broadcast for sending. */
    ENET_BuildBroadCastFrame();

    while (1)
    {
        /* Get the Frame size */
        status = ENET_GetRxFrameSize(&g_handle, &length, 0);
        /* Call ENET_ReadFrame when there is a received frame. */
        if (length != 0)
        {
            /* Received valid frame. Deliver the rx buffer with the size equal to length. */
            uint8_t *data = (uint8_t *)malloc(length);
            status        = ENET_ReadFrame(EXAMPLE_ENET, &g_handle, data, length, 0, NULL);
            if (status == kStatus_Success)
            {
            	uint8_t equal = false;
                memcpy(&g_rx_src_mac[0], &data[0], 6U);
                for (uint8_t i = 0; i < 6; ++i) {
                	if(g_rx_src_mac[i] == g_macAddr[i]){
                		equal = true;
                	}
                	else{
                		equal = false;
                		break;
                	}
				}
                if(equal){
                	PRINTF(" A frame received. the length %d ", length);
					PRINTF(" Dest Address %02x:%02x:%02x:%02x:%02x:%02x Src Address %02x:%02x:%02x:%02x:%02x:%02x \r\n",
						   data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9],
						   data[10], data[11]);
                }
                else{
                	PRINTF(" MAC ADDRES NOT K66\r\n");
                }
            }
            free(data);
        }
        else if (status == kStatus_ENET_RxFrameError)
        {
            /* Update the received buffer when error happened. */
            /* Get the error information of the received g_frame. */
            ENET_GetRxErrBeforeReadFrame(&g_handle, &eErrStatic, 0);
            /* update the receive buffer. */
            ENET_ReadFrame(EXAMPLE_ENET, &g_handle, NULL, 0, 0, NULL);
        }

        if (testTxNum < ENET_TRANSMIT_DATA_NUM)
        {
            /* Send a multicast frame when the PHY is link up. */
            if (kStatus_Success == PHY_GetLinkStatus(&phyHandle, &link))
            {
                if (link)
                {
                    testTxNum++;
                    if (kStatus_Success ==
                        ENET_SendFrame(EXAMPLE_ENET, &g_handle, &g_frame[0], ENET_DATA_LENGTH, 0, false, NULL))
                    {
                        PRINTF("The %d frame transmitted success!\r\n", testTxNum);
                    }
                    else
                    {
                        PRINTF(" \r\nTransmit frame failed!\r\n");
                    }
                }
            }
        }
    }
}


//#include "Drivers/Security.h"
//
//uint8_t buffer1[32] = "No todo lo que es oro reluce...";
//
//uint8_t buffer2[32];
//uint8_t buffer3[32];
//
//int main (void){
//	Security_init();
//	buffer1[31] = '0';
//
//	Security_send(buffer1, buffer2);
//
//	Security_receive(buffer2, buffer3);
//
//	while(1);
//}

