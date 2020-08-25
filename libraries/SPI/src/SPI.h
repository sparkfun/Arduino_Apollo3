/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#ifndef _APOLLO3_LIBRARIES_SPI_H_
#define _APOLLO3_LIBRARIES_SPI_H_

#include "Arduino.h"

#include "drivers/SPI.h"

namespace arduino {

class MbedSPI : public SPIClass
{
public:
    MbedSPI(pin_size_t miso, pin_size_t mosi, pin_size_t sck);
    MbedSPI(PinName miso, PinName mosi, PinName sck);
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
    PinName _miso;
    PinName _mosi;
    PinName _sck;
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
