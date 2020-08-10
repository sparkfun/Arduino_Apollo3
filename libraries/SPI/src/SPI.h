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

#ifndef _APOLLO3_LIBRARIES_SPI_H_
#define _APOLLO3_LIBRARIES_SPI_H_

#include "Arduino.h"

#include "drivers/SPI.h"

namespace arduino {

class MbedSPI : public SPIClass
{
public:
    MbedSPI(int miso, int mosi, int sck);
    virtual uint8_t transfer(uint8_t data);
    virtual uint16_t transfer16(uint16_t data);
    virtual void transfer(void *buf, size_t count);

    // Transaction Functions
    virtual void usingInterrupt(int interruptNumber);
    virtual void notUsingInterrupt(int interruptNumber);
    virtual void beginTransaction(SPISettings settings);
    virtual void endTransaction(void);

    // SPI Configuration methods
    virtual void attachInterrupt();
    virtual void detachInterrupt();

    virtual void begin();
    virtual void end();

private:
    SPISettings settings = SPISettings(0, MSBFIRST, SPI_MODE0);
    mbed::SPI* dev;
    int _miso;
    int _mosi;
    int _sck;
};

}

#if VARIANT_SPI_INTFCS > 0
extern arduino::MbedSPI SPI;
#endif
#if VARIANT_SPI_INTFCS > 1
extern arduino::MbedSPI SPI1;
#endif
#if VARIANT_SPI_INTFCS > 2
extern arduino::MbedSPI SPI2;
#endif
#if VARIANT_SPI_INTFCS > 3
extern arduino::MbedSPI SPI3;
#endif
#if VARIANT_SPI_INTFCS > 4
extern arduino::MbedSPI SPI4;
#endif
#if VARIANT_SPI_INTFCS > 5
extern arduino::MbedSPI SPI5;
#endif

#endif // _APOLLO3_LIBRARIES_SPI_H_
