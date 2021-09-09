/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#ifndef _VARIANT_H_
#define _VARIANT_H_

#include "Arduino.h"

#include "config/pins.h"

#define VARIANT_SPI_INTFCS 0
#define VARIANT_WIRE_INTFCS 1

// Add a default wire pin so that Wire is available to libraries and sketches
// Since this is just a module no pins are labeled I2C/Wire/Qwiic by default
// 2 pins have been chosen here arbitrarily
#define VARIANT_Wire_SDA 9
#define VARIANT_Wire_SCL 8

#endif // _VARIANT_H_
