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

#define AP3_VARIANT_NUM_PINS (32)

// Pin map declaration
extern const ap3_gpio_pad_t ap3_variant_pinmap[AP3_VARIANT_NUM_PINS];

// Uart declarations
class Uart; // Forward declaration of Uart
extern Uart Serial;
extern Uart Serial1;

// Wire defines
#define WIRE_INTERFACES_COUNT 4

#define WireQwiic Wire // Giving Wire an alias of "WireQwiic" in case people want to use it
#define AP3_Wire_IOM 4 // Secify that Wire uses IOMaster instance 4

#define AP3_Wire1_IOM 2
#define AP3_Wire2_IOM 1
#define AP3_Wire3_IOM 3

// SPI Defines
#define SPI_INTERFACES_COUNT 4

#define AP3_SPI_IOM 0                   // Specify that SPI uses IOMaster 0
#define AP3_SPI_DUP ap3_spi_full_duplex // Specify that SPI is full-duplex (as opposed to ap3_spi_tx_only or ap3_spi_rx_only)

#define AP3_SPI1_IOM 2
#define AP3_SPI1_DUP ap3_spi_full_duplex

#define AP3_SPI2_IOM 1
#define AP3_SPI2_DUP ap3_spi_full_duplex

#define AP3_SPI3_IOM 3
#define AP3_SPI3_DUP ap3_spi_full_duplex

// Mapping of analog pins to digital pins of variant
// (this method - aliasing the name of a constant variable - is compatible with the usage of 'AX' names in 'arm_math.h')
#define A0 ap3_analog_A0
#define A1 ap3_analog_A1
#define A2 ap3_analog_A2
#define A3 ap3_analog_A3
#define A4 ap3_analog_A4
#define A5 ap3_analog_A5
//No A7 for better alignment
#define A6 ap3_analog_A6
#define A8 ap3_analog_A8
#define A9 ap3_analog_A9
#define A10 ap3_analog_A10
#define ADIFF0 ap3_analog_DIFF0
#define ADIFF1 ap3_analog_DIFF1

// Promise the existence of analog pin names
extern const ap3_gpio_pin_t ap3_analog_A0;
extern const ap3_gpio_pin_t ap3_analog_A1;
extern const ap3_gpio_pin_t ap3_analog_A2;
extern const ap3_gpio_pin_t ap3_analog_A3;
extern const ap3_gpio_pin_t ap3_analog_A4;
extern const ap3_gpio_pin_t ap3_analog_A5;
extern const ap3_gpio_pin_t ap3_analog_A6;
//No A7 for better alignment
extern const ap3_gpio_pin_t ap3_analog_A8;
extern const ap3_gpio_pin_t ap3_analog_A9;
extern const ap3_gpio_pin_t ap3_analog_A10;
extern const ap3_gpio_pin_t ap3_analog_DIFF0;
extern const ap3_gpio_pin_t ap3_analog_DIFF1;

#define LED_BUILTIN 13

// Pins with dedicated silk covering the pin number
#define MOSI 7
#define MISO 6
#define SCK 5
#define SCL 15
#define SDA 14
#define TX1 1
#define RX1 0

#define MIC_DATA 30
#define MIC_CLOCK 31

#endif // _AP3_VARIANT_H_