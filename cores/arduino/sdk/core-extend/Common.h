/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
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
