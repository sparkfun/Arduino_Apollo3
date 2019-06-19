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
#include "ap3_timing.h"

volatile uint32_t ap3_stimer_overflows = 0x00;
uint64_t ticks = 0;

void _fill_ticks( void ){
    ticks = ap3_stimer_overflows;
    ticks <<= 32;
    ticks |= (am_hal_stimer_counter_get() & 0xFFFFFFFF);
}

unsigned long micros( void ){
    _fill_ticks();
    return (uint32_t)(ticks/AP3_STIMER_FREQ_MHZ);
}

unsigned long millis( void ){
    _fill_ticks();
    return (uint32_t)(ticks/AP3_STIMER_FREQ_KHZ);
}

unsigned long seconds( void ){
    _fill_ticks();
    return (uint32_t)(ticks/AP3_STIMER_FREQ_HZ);
}

unsigned long systicks( void ){
    return am_hal_stimer_counter_get();
}

unsigned long sysoverflows( void ){
    return ap3_stimer_overflows;
}

void delay(uint32_t ms){
    am_util_delay_ms(ms);
}

void delayMicroseconds(uint32_t us){
    am_util_delay_us(us);
}

extern "C" void am_stimer_isr(void){
  am_hal_stimer_int_clear(AM_HAL_STIMER_INT_OVERFLOW);
  ap3_stimer_overflows += 1;
  // At the fastest rate (3MHz) the 64 bits of the stimer 
  // along with this overflow counter can keep track of
  // the time for ~ 195,000 years without wrapping to 0
}

