/*
Copyright (c) 2019 SparkFun Electronics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef _AP3_VARIANT_H_
#define _AP3_VARIANT_H_

#include "Arduino.h"

#define AP3_VARIANT_NUM_PINS (50)

// Pin map declaration
extern const ap3_gpio_pad_t ap3_variant_pinmap[AP3_VARIANT_NUM_PINS];

// Uart declarations
class Uart; // Forward declaration of Uart
extern Uart Serial;

// Wire defines
#define WIRE_INTERFACES_COUNT 1

#define WireQwiic Wire // Giving Wire an alias of "WireQwiic" in case people want to use it
#define AP3_Wire_IOM 1 // Secify that Wire uses IOMaster instance 4

// SPI Defines
#define SPI_INTERFACES_COUNT 1

#define AP3_SPI_IOM 4                   // Specify that SPI uses IOMaster 4
#define AP3_SPI_DUP ap3_spi_full_duplex // Specify that SPI is full-duplex (as opposed to ap3_spi_tx_only or ap3_spi_rx_only)

// Mapping of analog pins to digital pins of variant
// (this method - aliasing the name of a constant variable - is compatible with the usage of 'AX' names in 'arm_math.h')
#define A13 ap3_analog_A13
#define A16 ap3_analog_A16
#define A29 ap3_analog_A29
#define A31 ap3_analog_A31
#define A32 ap3_analog_A32
#define A33 ap3_analog_A33
#define A34 ap3_analog_A34
#define A35 ap3_analog_A35
#define ADIFF0 ap3_analog_DIFF0
#define ADIFF1 ap3_analog_DIFF1

// Promise the existence of analog pin names
extern const ap3_gpio_pin_t ap3_analog_A13;
extern const ap3_gpio_pin_t ap3_analog_A16;
extern const ap3_gpio_pin_t ap3_analog_A29;
extern const ap3_gpio_pin_t ap3_analog_A31;
extern const ap3_gpio_pin_t ap3_analog_A32;
extern const ap3_gpio_pin_t ap3_analog_A33;
extern const ap3_gpio_pin_t ap3_analog_A34;
extern const ap3_gpio_pin_t ap3_analog_A35;
extern const ap3_gpio_pin_t ap3_analog_DIFF0;
extern const ap3_gpio_pin_t ap3_analog_DIFF1;

#define LED_BUILTIN 23

#define MIC_DATA 11
#define MIC_CLOCK 12

#endif // _AP3_VARIANT_H_
