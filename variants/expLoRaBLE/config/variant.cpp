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

#include "variant.h"

const ap3_gpio_pad_t ap3_variant_pinmap[AP3_VARIANT_NUM_PINS] = {
    //~ = PWM, A = ADC
    //Apollo Pad,      //Silkscreen indicator - Pin functions
    19,                //0     - ~RX1
    18,                //1     - ~TX1
    41,                //2     - ~/TX0/SWO/I2SWCLK
    31,                //3/A6  - ~A/RX0
    10,                 //4     - ~TX1/MOSI1/PDMCLK
    30,                //5     - ~I2SDAT/TX0
    37,                //6     - ~TX1/PDMCLK
    24,                //7     - ~TX1/32KHZ
    46,                //8     - ~I2SBCLK/PDMCLK/TX1
    33,                //9/A7  - ~A/SWO/32KHZ
    48,                //10    -  ~SWCH/TX0/SCL5
    28,                 //11    - ~MOSI2
    25,                 //12    - ~MISO2/SDA2
    27,                 //13    - ~SCK2/SCL2
    6,                //14    -  SDA0/MISO0/I2SDAT
    5,                //15    - ~SCL0/SCK0
    9,                //16    - ~SDA1/MISO1/RX1
    8,                //17    - ~SCL1/SCK1/TX1
    26,                //18    - ~LED/SCCRST
    13,                //19/A0 - ~A/I2SBCLK/RX1
    12,                //20/A1 - ~A/PDMCLK/TX1
    32,                //21/A2 - ~A
    35,                //22/A3 - ~A/I2SDAT
    34,                //23/A4 - ~A/PDMCLK/RX0
	11,                //24/A5 - ~A/PDMDAT/RX0
	22,                //25    - Not exposed, TX0
	23,                //26    - Not exposed, RX0
	36, 		//INTERNAL, RADIO NSS 
	38, 		//INTERNAL, RADIO_MOSI
	39, 		//INTERNAL, RADIO_BUSY
	40, 		//INTERNAL, RADIO_DIO1
	42, 		//INTERNAL, RADIO_CLK
	43, 		//INTERNAL, RADIO_MISO
	44, 		//INTERNAL, RADIO_nRESET
	47, 		//INTERNAL, RADIO_DIO3
};

// Uart Definitions
//Serial(instance, RX, TX)
Uart Serial(0, 26, 25); // Declares a Uart object called Serial using instance 0 of Apollo3 UART peripherals with RX on variant pin 26 and TX on pin 25 (note, you specify *pins* not Apollo3 pads. This uses the variant's pin map to determine the Apollo3 pad)
Uart Serial1(1, 0, 1);  // Declares a Uart object called Serial1 using instance 1 of Apollo3 UART peripherals with RX on pin 0 and TX on pin 1 (note, you specify *pins* not Apollo3 pads. This uses the variant's pin map to determine the Apollo3 pad)

// Analog pin numbers
const ap3_gpio_pin_t ap3_analog_A0 = 19;
const ap3_gpio_pin_t ap3_analog_A1 = 20;
const ap3_gpio_pin_t ap3_analog_A2 = 21;
const ap3_gpio_pin_t ap3_analog_A3 = 22;
const ap3_gpio_pin_t ap3_analog_A4 = 23;
const ap3_gpio_pin_t ap3_analog_A5 = 24;
const ap3_gpio_pin_t ap3_analog_A6 = 3;
const ap3_gpio_pin_t ap3_analog_A7 = 9;
const ap3_gpio_pin_t ap3_analog_DIFF0 = AP3_ADC_DIFF0_PAD;
