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

// IOMaster is a parent class that contains SPI and I2C functionality for the Apollo3

#ifndef _AP3_IOMASTER_H_
#define _AP3_IOMASTER_H_

#include "Arduino.h"
#include "ap3_iomaster_types.h"

class IOMaster {
private:
protected:
    uint8_t             _instance;
    void*               _handle;
    am_hal_iom_config_t _config;

public:
    IOMaster(uint8_t instance);
    ap3_err_t initialize( void );
    ap3_err_t initialize(am_hal_iom_config_t config);
    ap3_err_t deinitialize( void );
};



ap3_err_t ap3_iom_pad_funcsel( uint8_t instance, ap3_iom_pad_type_e type, ap3_gpio_pad_t* pad, uint8_t* funcsel );

extern "C" uint32_t ap3_iom_configure(void *pHandle, am_hal_iom_config_t *psConfig);


#endif // _AP3_IOMASTER_H_