#include "ap3_timing.h"

void delay(uint32_t ms){
    am_util_delay_ms(ms);
}

void delayMicroseconds(uint32_t us){
    am_util_delay_us(us);
}