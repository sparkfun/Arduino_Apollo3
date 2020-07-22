/*
Copyright (c) 2020 SparkFun Electronics

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

#ifndef _ARDUINO_SDK_CORE_EXTEND_COMMON_H_
#define _ARDUINO_SDK_CORE_EXTEND_COMMON_H_

#include "sdk/core-extend/ap3_types.h"

ap3_err_t analogWriteResolution(uint8_t bits);
ap3_err_t analogWriteFrameWidth(uint32_t width);
ap3_err_t analogWriteFrequency(float freq);

uint8_t getServoResolution();
ap3_err_t servoWriteResolution(uint8_t bits);
ap3_err_t servoWrite(uint8_t pin, uint32_t val);
ap3_err_t servoWrite(uint8_t pin, uint32_t val, uint16_t minMicros, uint16_t maxMicros);

ap3_err_t analogReadResolution(uint8_t bits);

#endif // _ARDUINO_SDK_CORE_EXTEND_COMMON_H_
