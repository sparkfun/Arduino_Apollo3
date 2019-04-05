#ifndef _ARDUINO_H_
#define _ARDUINO_H_


#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

    // Include Apollo headers
    #include "am_mcu_apollo.h"
    #include "am_util.h"
    #include "am_bsp.h"
    #include "ap3_types.h"

    /* system functions */
    int main(void);
    // void init(void);

    /* sketch */
    void setup(void);
    void loop(void);


#ifdef __cplusplus
} // extern "C"
#endif

#include "variant.h"
#include "ap3_gpio.h"

// constants
#define LOW             (0x0)
#define HIGH            (0x1)


#endif // _ARDUINO_H_
