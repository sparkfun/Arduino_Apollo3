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

/*

This is an example variant file for use with the SparkFun Apollo3 Arduino core.

This file is for the 'Artemis Module' variant. The Artemis module is intended to
be used as an Apollo3 microcontroller that conveniently includes most necessary 
support circuitry. Due to that fact many of the standard features of a variant.h
file are unecessary. Such features will be commented out but retained and
explained so that this file can serve as a starting point for creating new variants

*/

#ifndef _AP3_VARIANT_H_
#define _AP3_VARIANT_H_

#include "Arduino.h"

// In the Apollo3 Arduino core *pins* refer to the connections that can be accessed
// via Arduino code. This may be, for example, an exposed connection that the user could
// utilize directly or it may be a connection to an onboard sensor.
//
// *pads* on the other hand refer directly to the pad numbers of the Apollo3 microcontroller
//
// When developing a variant you might connect *pin* x of your board to *pad* y on the
// Apollo3. When a user asks to write to *pin* x the core will use a map to determine which
// *pad* to control.
//
// AP3_VARIANT_NUM_PINS determines the size of this map and its value must be N+1 where N
// is the largest _pin_id_ of your variant. Let's try an example:
//
// Variant has four *pins* to use in the Arduino core named: 0, 1, 2, and 3
// - in this case AP3_VARIANT_NUM_PINS is 4 (N+1, N=3)
// - this is good because there are no unused pin spaces in the map
//
// Variant has four *pins* to use in Arduino named: 7, 13, 15, 48
// - in this case AP3_VARIANT_NUM_PINS is 49 (N+1, N=48)
// - this is less ideal because there are 45 unused spaces (bytes) in the map
#define AP3_VARIANT_NUM_PINS (50)

// Pin map declaration
// The pinmap converts Arduino *pins* to Apollo3 *pads*
// The *pin* is the index into the array and the *pad* is the value at that index
extern const ap3_gpio_pad_t ap3_variant_pinmap[AP3_VARIANT_NUM_PINS];

// Uart declarations
// - "HardwareSerial" is the class that is built into Arduino to allow "Serial.print()"
// and related functions to work.
// - The "Uart" class inherits from HardwareSerial and contains Apollo3-specific code
// - Declaring your Uart objects here makes them available in any Arduino code and you
// can give them extra descriptive names
// - The maximum number of Uarts is 2 (instances 0 and 1)
//
// In this case the declarations are commented out because we don't know which pins of
// the Artemis module you would like to use
class Uart; // Forward declaration of Uart class
extern Uart Serial;
// extern Uart Serial1;

// A note about IOMaster peripherals:
// - IOMaster peripherals are used to provide either SPI or I2C communications. The pads
// of the Apollo3 that a given IOMaster [0,5] use are not configurable

// Wire defines
// - Wire is the I2C class for Arduino.
// - Wire is handled differently than HardwareSerial/Uart because Wire is a library that
// you have to include manually (saves space if you won't be using I2C)
// - In this case we rely on communication between the variant.h file and the Wire library
// through the use of #define statements
//
// - Define the total number of TwoWire class objects that you will allow the user on the board.
// - The maximum number is 6 because the Apollo3 incudes 6 IOMaster peripherals
#define WIRE_INTERFACES_COUNT 6
//
// - Define settings for as many Wire objects as you defined above, the only required value AP3_Wire[X]_IOM
// - Note: in this example each Wire object uses the same number IOMaster but that is not required.
// Any "#define AP3_Wire[X]_IOM Y" statement is valid for Y [0,5] and X [1,5] (or no X for the first "Wire")
// - Note: Wire objects will appear in order, so if WIRE_INTERFACES_COUNT = 3 then you must define the IOM #
// for Wire, Wire1, and Wire2
#define AP3_Wire_IOM 0  // Secify that Wire uses IOMaster instance 0
#define AP3_Wire1_IOM 1 // Secify that Wire1 uses IOMaster instance 1
#define AP3_Wire2_IOM 2 // Secify that Wire2 uses IOMaster instance 2
#define AP3_Wire3_IOM 3 // Secify that Wire3 uses IOMaster instance 3
#define AP3_Wire4_IOM 4 // Secify that Wire4 uses IOMaster instance 4
#define AP3_Wire5_IOM 5 // Secify that Wire5 uses IOMaster instance 5
// This is also a convenient location to provide some aliased names for certain Wire objects
// For example: (examples commented out because they aren't real)
//
// #define WireQwiic Wire  // Giving Wire an alias of "WireQwiic" to indicat that it is the I2C controller for the Qwiic bus
// #define WireAccel Wire1 // Useful if the variant has an onboard accelerometer connected to the Wire1 bus

// SPI Defines
// Like Wire SPI is an optional library so we have to rely on #define statements
// - Define the total number of SPI class objects that you will allow the user on the board.
// - The maximum number is 6 because the Apollo3 incudes 6 IOMaster peripherals
#define SPI_INTERFACES_COUNT 6
//
// - For each defined SPI interface (SPI, SPI1, SPI2, SPI3, SPI4, SPI5) in order up to SPI_INTERFACES_COUNT
// you need to specify two settings:
// - - AP3_SPI_IOM - which IOMaster peripher the SPI object will use
// - - AP3_SPI_DUP - which duplex mode the SPI object will use (full duplex is the most common, ap3_spi_tx_only and ap3_spi_rx_only are the other options )
#define AP3_SPI_IOM 0
#define AP3_SPI_DUP ap3_spi_full_duplex
#define AP3_SPI1_IOM 1
#define AP3_SPI1_DUP ap3_spi_full_duplex
#define AP3_SPI2_IOM 2
#define AP3_SPI2_DUP ap3_spi_full_duplex
#define AP3_SPI3_IOM 3
#define AP3_SPI3_DUP ap3_spi_full_duplex
#define AP3_SPI4_IOM 4
#define AP3_SPI4_DUP ap3_spi_full_duplex
#define AP3_SPI5_IOM 5
#define AP3_SPI5_DUP ap3_spi_full_duplex

// Additional Pin Aliasing
// - It is required that every pin is accessible by a number between 0 and (AP3_VARIANT_NUM_PINS - 1)
// - You are optionally allowed to provide other ways to refer to those pin #s
// - - One way is to use a #define. This is good because it takes no program space, but you should be
//   careful of name conflicts
// - - Another way is to use a const uint8_t variable. This will allow the compiler to warn you about
//   name conflicts but it will use a byte of program space
// - You should alias Arduino *pin* numbers, not Apollo3 *pad* numbers
//
// Here is an example of how the #define technique is used to match the available pins of an Arduino Uno
//
// // Mapping of analog pins to digital pins of variant
// #define A0 16    // means that "A0" is equivalent to saying *pin* 16 (which culd map to any Apollo3 *pad*)
// #define A1 17
// #define A2 18
// #define A3 19
// #define A4 20
// #define A5 21
//
// When using the const uint8_t method you'll need to use 'extern'
// extern const uint8_t A0;
// extern const uint8_t A1;
// extern const uint8_t A2;
// extern const uint8_t A3;
// extern const uint8_t A4;
// extern const uint8_t A5;

// Many Arduino cores provide a built-in LED for quick verification, like in the Blink sketch
#define LED_BUILTIN 13

// Artemis has a variety of PDM capable pins. These defines can be modified, even excluded
// and declared during myPDM.begin(pdm_data, pdm_clock)
#define MIC_DATA 36
#define MIC_CLOCK 37

#endif // _AP3_VARIANT_H_