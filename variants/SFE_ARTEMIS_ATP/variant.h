/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#ifndef _VARIANT_H_
#define _VARIANT_H_

#include "Arduino.h"

#include "config/pins.h"

#define VARIANT_SPI_INTFCS  1
#define VARIANT_WIRE_INTFCS 1

// SPI
#define VARIANT_SPI_SDI     SPI_SDI
#define VARIANT_SPI_SDO     SPI_SDO
#define VARIANT_SPI_CLK     SPI_CLK

// Wire
#define VARIANT_Wire_SDA    I2C_SDA
#define VARIANT_Wire_SCL    I2C_SCL

// UART
extern UART Serial1;

// temporary patch to support Arduino SD library
#define SS 0
#define MOSI SPI_SDO
#define MISO SPI_SDI
#define SCK SPI_CLK

#endif // _VARIANT_H_
