//*****************************************************************************
//
//  am_bsp_pins.h
//! @file
//!
//! @brief BSP pin configuration definitions.
//!
//! @addtogroup BSP Board Support Package (BSP)
//! @addtogroup apollo3_bsp BSP for the Apollo3 EVB.
//! @ingroup BSP
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2019, Ambiq Micro
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
// 
// Third party software included in this distribution is subject to the
// additional license terms as defined in the /docs/licenses directory.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision 2.2.0-hotfix-2.2.1 of the AmbiqSuite Development Package.
//
//*****************************************************************************

#ifndef AM_BSP_PINS_H
#define AM_BSP_PINS_H

#include <stdint.h>
#include <stdbool.h>
#include "am_mcu_apollo.h"

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//  AUD_MCLK pin.
//
//*****************************************************************************
#define AM_BSP_GPIO_AUD_MCLK            	18
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_AUD_MCLK;

//*****************************************************************************
//
//  AUD_OUT pin.
//
//*****************************************************************************
#define AM_BSP_GPIO_AUD_OUT             	18
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_AUD_OUT;

//*****************************************************************************
//
//  AUD_IN pin.
//
//*****************************************************************************
#define AM_BSP_GPIO_AUD_IN              	11
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_AUD_IN;

//*****************************************************************************
//
//  AUD_LRCLK pin.
//
//*****************************************************************************
#define AM_BSP_GPIO_AUD_LRCLK           	36
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_AUD_LRCLK;

//*****************************************************************************
//
//  AUD_BCLK pin.
//
//*****************************************************************************
#define AM_BSP_GPIO_AUD_BCLK            	37
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_AUD_BCLK;

//*****************************************************************************
//
//  SCL pin: I/O Master 4 I2C clock signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_SCL                 	39
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_SCL;

//*****************************************************************************
//
//  SDA pin: I/O Master 4 I2C data signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDA                 	40
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_SDA;

//*****************************************************************************
//
//  I2C_INT pin.
//
//*****************************************************************************
#define AM_BSP_GPIO_I2C_INT             	2
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_I2C_INT;

//*****************************************************************************
//
//  SCL1 pin: I/O Master 1 I2C clock signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_SCL1                	8
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_SCL1;

//*****************************************************************************
//
//  SDA1 pin: I/O Master 1 I2C data signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDA1                	9
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_SDA1;

//*****************************************************************************
//
//  BATT_VIN pin.
//
//*****************************************************************************
#define AM_BSP_GPIO_BATT_VIN            	31
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_BATT_VIN;

//*****************************************************************************
//
//  SDIO_CLK pin: I/O Master 0 SPI SCK signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO_CLK            	5
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_SDIO_CLK;

//*****************************************************************************
//
//  SDIO_CMD pin: I/O Master 0 SPI SDO signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO_CMD            	7
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_SDIO_CMD;

//*****************************************************************************
//
//  SDIO_DATA0 pin: I/O Master 0 SPI SDI signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO_DATA0          	6
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_SDIO_DATA0;

//*****************************************************************************
//
//  SDIO_DATA1 pin: QSPI data1 signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO_DATA1          	26
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_SDIO_DATA1;

//*****************************************************************************
//
//  SDIO_DATA2 pin: QSPI data2 signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO_DATA2          	4
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_SDIO_DATA2;

//*****************************************************************************
//
//  SDIO_DATA3 pin: QSPI data3 signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO_DATA3          	23
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_SDIO_DATA3;

//*****************************************************************************
//
//  SPI_SDI pin: I/O Master 3 SPI SDI signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_SPI_SDI             	43
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_SPI_SDI;

//*****************************************************************************
//
//  SPI_SDO pin: I/O Master 3 SPI SDO signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_SPI_SDO             	38
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_SPI_SDO;

//*****************************************************************************
//
//  SPI_SCK pin: I/O Master 3 SPI SCK signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_SPI_SCK             	42
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_SPI_SCK;

//*****************************************************************************
//
//  SPI_CS pin: SPI CS signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_SPI_CS              	41
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_SPI_CS;
#define AM_BSP_SPI_CS_CHNL              0

//*****************************************************************************
//
//  SPI_SDI1 pin: I/O Master 0 SPI SDI signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_SPI_SDI1            	6
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_SPI_SDI1;

//*****************************************************************************
//
//  SPI_SDO1 pin: I/O Master 0 SPI SDO signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_SPI_SDO1            	7
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_SPI_SDO1;

//*****************************************************************************
//
//  SPI_SCK1 pin: I/O Master 0 SPI SCK signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_SPI_SCK1            	5
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_SPI_SCK1;

//*****************************************************************************
//
//  SPI_CS1 pin: SPI1 CS signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_SPI_CS1             	23
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_SPI_CS1;
#define AM_BSP_SPI_CS1_CHNL             0

//*****************************************************************************
//
//  A0 pin.
//
//*****************************************************************************
#define AM_BSP_GPIO_A0                  	32
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_A0;

//*****************************************************************************
//
//  A1 pin.
//
//*****************************************************************************
#define AM_BSP_GPIO_A1                  	35
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_A1;

//*****************************************************************************
//
//  PWM0 pin.
//
//*****************************************************************************
#define AM_BSP_GPIO_PWM0                	44
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_PWM0;

//*****************************************************************************
//
//  PWM1 pin.
//
//*****************************************************************************
#define AM_BSP_GPIO_PWM1                	45
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_PWM1;

//*****************************************************************************
//
//  D0 pin.
//
//*****************************************************************************
#define AM_BSP_GPIO_D0                  	0
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_D0;

//*****************************************************************************
//
//  D1 pin.
//
//*****************************************************************************
#define AM_BSP_GPIO_D1                  	1
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_D1;

//*****************************************************************************
//
//  TX1 pin: This pin is the UART1 transmit pin.
//
//*****************************************************************************
#define AM_BSP_GPIO_TX1                 	12
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_TX1;

//*****************************************************************************
//
//  RX1 pin: This pin is the UART1 receive pin.
//
//*****************************************************************************
#define AM_BSP_GPIO_RX1                 	25
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_RX1;

//*****************************************************************************
//
//  RTS1 pin: This pin is the UART1 rts pin.
//
//*****************************************************************************
#define AM_BSP_GPIO_RTS1                	10
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_RTS1;

//*****************************************************************************
//
//  CTS1 pin: This pin is the UART1 cts pin.
//
//*****************************************************************************
#define AM_BSP_GPIO_CTS1                	17
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_CTS1;

//*****************************************************************************
//
//  G0 pin.
//
//*****************************************************************************
#define AM_BSP_GPIO_G0                  	16
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_G0;

//*****************************************************************************
//
//  G1 pin.
//
//*****************************************************************************
#define AM_BSP_GPIO_G1                  	33
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_G1;

//*****************************************************************************
//
//  G2 pin.
//
//*****************************************************************************
#define AM_BSP_GPIO_G2                  	34
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_G2;

//*****************************************************************************
//
//  G3 pin.
//
//*****************************************************************************
#define AM_BSP_GPIO_G3                  	27
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_G3;

//*****************************************************************************
//
//  G4 pin.
//
//*****************************************************************************
#define AM_BSP_GPIO_G4                  	28
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_G4;

//*****************************************************************************
//
//  G5 pin.
//
//*****************************************************************************
#define AM_BSP_GPIO_G5                  	29
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_G5;

//*****************************************************************************
//
//  G6 pin.
//
//*****************************************************************************
#define AM_BSP_GPIO_G6                  	14
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_G6;

//*****************************************************************************
//
//  G7 pin.
//
//*****************************************************************************
#define AM_BSP_GPIO_G7                  	15
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_G7;

//*****************************************************************************
//
//  G9 pin.
//
//*****************************************************************************
#define AM_BSP_GPIO_G9                  	12
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_G9;

//*****************************************************************************
//
//  G10 pin.
//
//*****************************************************************************
#define AM_BSP_GPIO_G10                 	13
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_G10;

//*****************************************************************************
//
//  MIC_DATA pin: Data line for PDM microphones.
//
//*****************************************************************************
#define AM_BSP_GPIO_MIC_DATA            	36
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_MIC_DATA;

//*****************************************************************************
//
//  MIC_CLK pin: Clock line for PDM microphones.
//
//*****************************************************************************
#define AM_BSP_GPIO_MIC_CLK             	37
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_MIC_CLK;

//*****************************************************************************
//
//  LED_BLUE pin: The BLUE LED labeled STAT.
//
//*****************************************************************************
#define AM_BSP_GPIO_LED_BLUE            	19
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_LED_BLUE;

//*****************************************************************************
//
//  COM_UART_TX pin: This pin is the COM_UART transmit pin.
//
//*****************************************************************************
#define AM_BSP_GPIO_COM_UART_TX         	48
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_COM_UART_TX;

//*****************************************************************************
//
//  COM_UART_RX pin: This pin is the COM_UART receive pin.
//
//*****************************************************************************
#define AM_BSP_GPIO_COM_UART_RX         	49
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_COM_UART_RX;

//*****************************************************************************
//
//  IOM0_CS pin: I/O Master 0 chip select.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM0_CS             	11
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM0_CS;
#define AM_BSP_IOM0_CS_CHNL             0

//*****************************************************************************
//
//  IOM0_CS3 pin: I/O Master 0 chip select.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM0_CS3            	15
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM0_CS3;
#define AM_BSP_IOM0_CS3_CHNL            3

//*****************************************************************************
//
//  IOM0_MISO pin: I/O Master 0 SPI MISO signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM0_MISO           	6
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM0_MISO;

//*****************************************************************************
//
//  IOM0_MOSI pin: I/O Master 0 SPI MOSI signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM0_MOSI           	7
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM0_MOSI;

//*****************************************************************************
//
//  IOM0_SCK pin: I/O Master 0 SPI SCK signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM0_SCK            	5
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM0_SCK;

//*****************************************************************************
//
//  IOM0_SCL pin: I/O Master 0 I2C clock signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM0_SCL            	5
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM0_SCL;

//*****************************************************************************
//
//  IOM0_SDA pin: I/O Master 0 I2C data signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM0_SDA            	6
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM0_SDA;

//*****************************************************************************
//
//  IOM1_CS pin: I/O Master 1 chip select.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM1_CS             	14
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM1_CS;
#define AM_BSP_IOM1_CS_CHNL             2

//*****************************************************************************
//
//  IOM1_MISO pin: I/O Master 1 SPI MISO signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM1_MISO           	9
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM1_MISO;

//*****************************************************************************
//
//  IOM1_MOSI pin: I/O Master 1 SPI MOSI signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM1_MOSI           	10
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM1_MOSI;

//*****************************************************************************
//
//  IOM1_SCK pin: I/O Master 1 SPI SCK signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM1_SCK            	8
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM1_SCK;

//*****************************************************************************
//
//  IOM1_SCL pin: I/O Master 1 I2C clock signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM1_SCL            	8
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM1_SCL;

//*****************************************************************************
//
//  IOM1_SDA pin: I/O Master 1 I2C data signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM1_SDA            	9
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM1_SDA;

//*****************************************************************************
//
//  IOM2_CS pin: I/O Master 2 chip select.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM2_CS             	15
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM2_CS;
#define AM_BSP_IOM2_CS_CHNL             3

//*****************************************************************************
//
//  IOM2_MISO pin: I/O Master 2 SPI MISO signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM2_MISO           	25
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM2_MISO;

//*****************************************************************************
//
//  IOM2_MOSI pin: I/O Master 2 SPI MOSI signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM2_MOSI           	28
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM2_MOSI;

//*****************************************************************************
//
//  IOM2_SCK pin: I/O Master 2 SPI SCK signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM2_SCK            	27
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM2_SCK;

//*****************************************************************************
//
//  IOM2_SCL pin: I/O Master 2 I2C clock signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM2_SCL            	27
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM2_SCL;

//*****************************************************************************
//
//  IOM2_SDA pin: I/O Master 2 I2C data signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM2_SDA            	25
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM2_SDA;

//*****************************************************************************
//
//  IOM3_CS pin: I/O Master 3 chip select.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM3_CS             	12
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM3_CS;
#define AM_BSP_IOM3_CS_CHNL             0

//*****************************************************************************
//
//  IOM3_MISO pin: I/O Master 3 SPI MISO signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM3_MISO           	43
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM3_MISO;

//*****************************************************************************
//
//  IOM3_MOSI pin: I/O Master 3 SPI MOSI signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM3_MOSI           	38
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM3_MOSI;

//*****************************************************************************
//
//  IOM3_SCK pin: I/O Master 3 SPI SCK signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM3_SCK            	42
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM3_SCK;

//*****************************************************************************
//
//  IOM3_SCL pin: I/O Master 3 I2C clock signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM3_SCL            	42
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM3_SCL;

//*****************************************************************************
//
//  IOM3_SDA pin: I/O Master 3 I2C data signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM3_SDA            	43
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM3_SDA;

//*****************************************************************************
//
//  IOM4_CS pin: I/O Master 4 chip select.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM4_CS             	13
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM4_CS;
#define AM_BSP_IOM4_CS_CHNL             1

//*****************************************************************************
//
//  IOM4_MISO pin: I/O Master 4 SPI MISO signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM4_MISO           	40
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM4_MISO;

//*****************************************************************************
//
//  IOM4_MOSI pin: I/O Master 4 SPI MOSI signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM4_MOSI           	44
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM4_MOSI;

//*****************************************************************************
//
//  IOM4_SCK pin: I/O Master 4 SPI SCK signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM4_SCK            	39
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM4_SCK;

//*****************************************************************************
//
//  IOM4_SCL pin: I/O Master 4 I2C clock signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM4_SCL            	39
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM4_SCL;

//*****************************************************************************
//
//  IOM4_SDA pin: I/O Master 4 I2C data signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM4_SDA            	40
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM4_SDA;

//*****************************************************************************
//
//  IOM5_CS pin: I/O Master 5 chip select.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM5_CS             	16
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM5_CS;
#define AM_BSP_IOM5_CS_CHNL             0

//*****************************************************************************
//
//  IOM5_MISO pin: I/O Master 5 SPI MISO signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM5_MISO           	49
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM5_MISO;

//*****************************************************************************
//
//  IOM5_MOSI pin: I/O Master 5 SPI MOSI signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM5_MOSI           	47
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM5_MOSI;

//*****************************************************************************
//
//  IOM5_SCK pin: I/O Master 5 SPI SCK signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM5_SCK            	48
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM5_SCK;

//*****************************************************************************
//
//  IOM5_SCL pin: I/O Master 5 I2C clock signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM5_SCL            	48
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM5_SCL;

//*****************************************************************************
//
//  IOM5_SDA pin: I/O Master 5 I2C data signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOM5_SDA            	49
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOM5_SDA;

//*****************************************************************************
//
//  MSPI_CE0 pin: MSPI chip select.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI_CE0            	19
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_MSPI_CE0;
#define AM_BSP_MSPI_CE0_CHNL            0

//*****************************************************************************
//
//  MSPI_CE1 pin: MSPI chip select.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI_CE1            	41
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_MSPI_CE1;
#define AM_BSP_MSPI_CE1_CHNL            1

//*****************************************************************************
//
//  MSPI_D0 pin: MSPI data 0.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI_D0             	22
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_MSPI_D0;

//*****************************************************************************
//
//  MSPI_D1 pin: MSPI data 1.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI_D1             	26
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_MSPI_D1;

//*****************************************************************************
//
//  MSPI_D2 pin: MSPI data 2.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI_D2             	4
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_MSPI_D2;

//*****************************************************************************
//
//  MSPI_D3 pin: MSPI data 3.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI_D3             	23
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_MSPI_D3;

//*****************************************************************************
//
//  MSPI_D4 pin: MSPI data 4.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI_D4             	0
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_MSPI_D4;

//*****************************************************************************
//
//  MSPI_D5 pin: MSPI data 5.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI_D5             	1
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_MSPI_D5;

//*****************************************************************************
//
//  MSPI_D6 pin: MSPI data 6.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI_D6             	2
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_MSPI_D6;

//*****************************************************************************
//
//  MSPI_D7 pin: MSPI data 7.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI_D7             	3
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_MSPI_D7;

//*****************************************************************************
//
//  MSPI_SCK pin: MSPI clock.
//
//*****************************************************************************
#define AM_BSP_GPIO_MSPI_SCK            	24
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_MSPI_SCK;

//*****************************************************************************
//
//  IOS_CE pin: I/O Slave chip select.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOS_CE              	3
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOS_CE;
#define AM_BSP_IOS_CE_CHNL              0

//*****************************************************************************
//
//  IOS_MISO pin: I/O Slave SPI MISO signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOS_MISO            	2
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOS_MISO;

//*****************************************************************************
//
//  IOS_MOSI pin: I/O Slave SPI MOSI signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOS_MOSI            	1
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOS_MOSI;

//*****************************************************************************
//
//  IOS_SCK pin: I/O Slave SPI SCK signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOS_SCK             	0
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOS_SCK;

//*****************************************************************************
//
//  IOS_SCL pin: I/O Slave I2C clock signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOS_SCL             	0
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOS_SCL;

//*****************************************************************************
//
//  IOS_SDA pin: I/O Slave I2C data signal.
//
//*****************************************************************************
#define AM_BSP_GPIO_IOS_SDA             	1
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_IOS_SDA;

//*****************************************************************************
//
//  SWDCK pin: Cortex Serial Wire DCK.
//
//*****************************************************************************
#define AM_BSP_GPIO_SWDCK               	20
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_SWDCK;

//*****************************************************************************
//
//  SWDIO pin: Cortex Serial Wire DIO.
//
//*****************************************************************************
#define AM_BSP_GPIO_SWDIO               	21
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_SWDIO;


#ifdef __cplusplus
}
#endif

#endif // AM_BSP_PINS_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
