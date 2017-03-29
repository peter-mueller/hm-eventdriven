/**
 * @file board.h
 *
 * @brief AT91Lib board specific functionality
 *
 * This file provides a number of definitions, which are needed by the AT91Lib
 * and therefore cannot be placed into the pal_config.h.
 *
 * $Id: board.h,v 1.1.4.1 2011/07/08 16:55:30 dam Exp $
 *
 * @author
 *      dresden elektronik: http://www.dresden-elektronik.de
 *      Support email: support@dresden-elektronik.de
 */
/*
 * Copyright (c) 2010, dresden elektronik All rights reserved.
 *
 * Licensed under dresden elektronik's Limited License Agreement --> deEULA.txt
 */

#ifndef BOARD_H
#define BOARD_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------
#include <AT91SAM7X512.h>

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Board
//------------------------------------------------------------------------------
/// String containing the name of the board.
#define BOARD_NAME      "deRFarm7_25X00_deRFgateway_1XXX2"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Clocks
//------------------------------------------------------------------------------
/// Frequency of the board main oscillator, in Hz.
#define BOARD_MAINOSC           18432000

/// Master clock frequency (when using board_lowlevel.c), in Hz.
#define BOARD_MCK               48000000
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// USB
//------------------------------------------------------------------------------
/// Chip has a UDP controller.
#define BOARD_USB_UDP

/// Indicates the D+ pull-up is always connected.
#define BOARD_USB_PULLUP_ALWAYSON

/// Number of endpoints in the USB controller.
#define BOARD_USB_NUMENDPOINTS                  6

/// Returns the maximum packet size of the given endpoint.
#define BOARD_USB_ENDPOINTS_MAXPACKETSIZE(i)    ((((i) == 4) || ((i) == 5)) ? 256 : (((i) == 0) ? 8 : 64))

/// Returns the number of FIFO banks for the given endpoint.
#define BOARD_USB_ENDPOINTS_BANKS(i)            ((((i) == 0) || ((i) == 3)) ? 1 : 2)

/// USB attributes configuration descriptor (bus or self powered, remote wakeup)
#define BOARD_USB_BMATTRIBUTES                  USBConfigurationDescriptor_BUSPOWERED_NORWAKEUP
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// PIO definitions
//------------------------------------------------------------------------------
// DBGU pins definition.
#define PINS_DBGU  {0x18000000, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}

/// LED 0 pin definition.
#define PIN_LED_0  {1 << 26, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT}
/// LED 1 pin definition.
#define PIN_LED_1  {1 << 21, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT}
/// LED 2 pin definition.
#define PIN_LED_2  {1 << 19, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT}
/// List of all LEDs pin definitions.
#define PINS_LEDS  PIN_LED_0, PIN_LED_1, PIN_LED_2

/// USB VBUS detection pin definition.
#if ((defined BOARD_REV00) || (defined BOARD_REV01))
#define PIN_USB_VBUS  {1 << 25, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_DEFAULT}
#else
#define PIN_USB_VBUS  {1 << 28, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_DEFAULT}
#endif

//-------------------------EMAC-----------------------------------------------------

/// Board EMAC Power Down control pin
#define BOARD_EMAC_PIN_PWRDN {(1<<18), AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
/// Board EMAC mode - RMII/MII ( 1/0 )
#define BOARD_EMAC_MODE_RMII 0 // MII is the default mode
/// The PIN list of PIO for EMAC
#define BOARD_EMAC_PINS     {0x3FFFF, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
/// The power up reset latch PIO for PHY
#define BOARD_EMAC_PIN_TEST   {(1<<15), AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define BOARD_EMAC_PIN_RMII   {(1<<16), AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
// We force the address
//(1<<5) PHY address 0, (1<<6) PHY address 1, (1<<13) PHY address 2,
//(1<<14) PHY address 3, (1<<4) PHY address 4
#define BOARD_EMAC_PINS_PHYAD {(1<<6)|(1<<13)|(1<<14)|(1<<4),\
                               AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT},\
                              {(1<<5), AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT}
#define BOARD_EMAC_PIN_10BT   {(1<<17), AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define BOARD_EMAC_PIN_RPTR   {(1<< 7), AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
/// The PIN Configure list for EMAC on power up reset (MII)
#define BOARD_EMAC_RST_PINS BOARD_EMAC_PINS_PHYAD, \
                            BOARD_EMAC_PIN_TEST, BOARD_EMAC_PIN_RMII, \
                            BOARD_EMAC_PIN_10BT, BOARD_EMAC_PIN_RPTR

/// The runtime pin configure list for EMAC
#define BOARD_EMAC_RUN_PINS BOARD_EMAC_PINS
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Flash
//------------------------------------------------------------------------------
/// Indicates chip has an EFC.
#define BOARD_FLASH_EFC
/// Address of the IAP function in ROM.
#define BOARD_FLASH_IAP_ADDRESS         0x300008
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Pin definitions to communicate with deRFarm7-25A00
// (use PIOA with PERIPH_B)
//------------------------------------------------------------------------------
/// SPI1 pin definitions
#define SPI1_MISO   {1 << 24, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_B, PIO_DEFAULT}		// PA24/RD/SPI1_MISO
#define SPI1_MOSI 	{1 << 23, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_B, PIO_DEFAULT}		// PA23/TD/SPI1_MOSI
#define SPI1_SPCK		{1 << 22, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_B, PIO_DEFAULT} 		// PA22/TK/SPI1_SPCK
#define SPI1_NPCS0  {1 << 21, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_B, PIO_DEFAULT} 		// PA21/TF/SPI1_NPCS0
/// List of all SPI pin definitions
//#define SPI1_PINS   SPI1_MOSI,SPI1_MISO,SPI1_SPCK,SPI1_NPCS0

/// IRQ
#define deRF_IRQ    {1 << 29, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT} 		// PA29/FIQ/SPI1_NPCS3
/// RST
#define deRF_RST    {1 << 9, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT} 		// PA9/CTS1/SPI0_NPCS3
/// SLP_TR
#define de_RF_SLP_TR	{1 << 8, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT} 		// PA8/RTS1/SPI0_NPCS2

//------------------------------------------------------------------------------

#endif //#ifndef BOARD_H

