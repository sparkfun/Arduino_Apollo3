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
#ifndef _AP3_TIMING_H_
#define _AP3_TIMING_H_

#include "Arduino.h"

#define AP3_STIMER_FREQ_HZ (3000000)
#define AP3_STIMER_FREQ_KHZ (AP3_STIMER_FREQ_HZ / 1000)
#define AP3_STIMER_FREQ_MHZ (AP3_STIMER_FREQ_HZ / 1000000)

unsigned long micros(void);
unsigned long millis(void);
unsigned long secs(void);
unsigned long systicks(void);
unsigned long sysoverflows(void);
void delay(uint32_t ms);
void delayMicroseconds(uint32_t us);

#endif // _AP3_TIMING_H_