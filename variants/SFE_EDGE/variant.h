/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#ifndef _VARIANT_H_
#define _VARIANT_H_

#include "Arduino.h"

#include "config/pins.h"

#define VARIANT_SPI_INTFCS  0
#define VARIANT_WIRE_INTFCS 1

// Wire
#define VARIANT_Wire_SDA    I2C_SDA
#define VARIANT_Wire_SCL    I2C_SCL

#endif // _VARIANT_H_
