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
extern Uart Serial1;

// Wire defines
#define WIRE_INTERFACES_COUNT 2

#define AP3_Wire_IOM 4 // Secify that Wire uses IOMaster instance 4
#define AP3_Wire1_IOM 1

// SPI Defines
#define SPI_INTERFACES_COUNT 2

#define AP3_SPI_IOM 3                   // Specify that SPI uses IOMaster 3
#define AP3_SPI_DUP ap3_spi_full_duplex // Specify that SPI is full-duplex (as opposed to ap3_spi_tx_only or ap3_spi_rx_only)

#define AP3_SPI1_IOM 0
#define AP3_SPI1_DUP ap3_spi_full_duplex

// Mapping of analog pins to digital pins of variant
// (this method - aliasing the name of a constant variable - is compatible with the usage of 'AX' names in 'arm_math.h')
#define A0 ap3_analog_A32
#define A1 ap3_analog_A35
#define BATTVIN3 ap3_analog_A31

// Promise the existence of analog pin names
extern const ap3_gpio_pin_t ap3_analog_A32;
extern const ap3_gpio_pin_t ap3_analog_A35;
extern const ap3_gpio_pin_t ap3_analog_A31;

#define LED_BUILTIN 19

#define POWER_CONTROL 3

// Pins with dedicated silk
#define PWM0 44
#define PWM1 45

#define D0 0
#define D1 1

#define MOSI1 7
#define MISO1 6 //Duplicate route to pad 22 as well.
#define SCK1 5
#define CS1 23

#define SDA1 9
#define SCL1 8
#define I2CINT 2

#define MOSI 38
#define MISO 43
#define SCK 42
#define CS 41

#define PDM_DATA 36
#define PDM_CLOCK 37

#define ADC_DIFF_NEG 12
#define ADC_DIFF_POS 13

#define G0 16
#define G1 33
#define G2 34
#define G3 27
#define G4 28
#define G5 29
#define G6 14
#define G7 15
#define G8 3
#define G9 ADC_DIFF_NEG
#define G10 ADC_DIFF_POS
#define G11 11

#endif // _AP3_VARIANT_H_
