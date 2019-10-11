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

#define AP3_VARIANT_NUM_PINS (29)

// Pin map declaration
extern const ap3_gpio_pad_t ap3_variant_pinmap[AP3_VARIANT_NUM_PINS];

// Uart declarations
class Uart; // Forward declaration of Uart
extern Uart Serial;
extern Uart Serial1;

// Wire defines
#define WIRE_INTERFACES_COUNT 2

#define WireQwiic Wire  // Giving Wire an alias of "WireQwiic" in case people want to use it
#define AP3_Wire_IOM 4  // Secify that Wire uses IOMaster instance 4
#define AP3_Wire1_IOM 3 // Secify that Wire1 uses IOMaster instance 3

// SPI Defines
#define SPI_INTERFACES_COUNT 1

#define AP3_SPI_IOM 0                   // Specify that SPI uses IOMaster 0
#define AP3_SPI_DUP ap3_spi_full_duplex // Specify that SPI is full-duplex (as opposed to ap3_spi_tx_only or ap3_spi_rx_only)

// Mapping of analog pins to digital pins of variant
#define A0 19
#define A1 20
#define A2 21
#define A3 22
#define A4 23
#define A5 24
#define A6 3
#define A7 9

#define LED_BUILTIN 18

// SPI Pins
#define MOSI 11
#define MISO 12
#define SCK 13
#define SS 10
// Wire Pins
#define SCL 15
#define SDA 14
// Wire1 Pins
#define SCL1 17
#define SDA1 16
// Serial1 Pins
#define TX1 1
#define RX1 0
// PDM Microphone Pins
#define MIC_DATA 27
#define MIC_CLOCK 28

#endif // _AP3_VARIANT_H_