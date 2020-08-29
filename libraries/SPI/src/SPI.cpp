/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "SPI.h"

arduino::MbedSPI::MbedSPI(pin_size_t miso, pin_size_t mosi, pin_size_t sck) {
    _miso = pinNameByNumber(miso);
    _mosi = pinNameByNumber(mosi);
    _sck = pinNameByNumber(sck);
}

arduino::MbedSPI::MbedSPI(PinName miso, PinName mosi, PinName sck) : _miso(miso), _mosi(mosi), _sck(sck) {
}

uint8_t arduino::MbedSPI::transfer(uint8_t data) {
    uint8_t ret;
    uint32_t tx = data;
    dev->write((const char*)&tx, 1, (char*)&ret, 1);
    return ret;
}

uint16_t arduino::MbedSPI::transfer16(uint16_t data) {

    union { uint16_t val; struct { uint8_t lsb; uint8_t msb; }; } t;
    t.val = data;

    if (settings.getBitOrder() == LSBFIRST) {
        t.lsb = transfer(t.lsb);
        t.msb = transfer(t.msb);
    } else {
        t.msb = transfer(t.msb);
        t.lsb = transfer(t.lsb);
    }
    return t.val;
}

void arduino::MbedSPI::transfer(void *buf, size_t count) {
    dev->write((const char*)buf, count, (char*)buf, count);
}

void arduino::MbedSPI::usingInterrupt(int interruptNumber) {

}

void arduino::MbedSPI::notUsingInterrupt(int interruptNumber) {

}

void arduino::MbedSPI::beginTransaction(SPISettings settings) {
    if(!dev){
        begin();
    }
    if (settings != this->settings) {
        dev->format(8, settings.getDataMode());
        dev->frequency(settings.getClockFreq());
        this->settings = settings;
    }
}

void arduino::MbedSPI::endTransaction(void) {
    // spinlock until transmission is over (if using ASYNC transfer)
}

void arduino::MbedSPI::attachInterrupt() {

}

void arduino::MbedSPI::detachInterrupt() {

}

void arduino::MbedSPI::begin() {
    if(!dev){
        dev = new mbed::SPI(_mosi, _miso, _sck);
    }
}

void arduino::MbedSPI::end() {
    if (!dev) {
        delete dev;
    }
}

#if VARIANT_SPI_INTFCS > 0
arduino::MbedSPI SPI(VARIANT_SPI_SDI, VARIANT_SPI_SDO, VARIANT_SPI_CLK);
#endif
#if VARIANT_SPI_INTFCS > 1
arduino::MbedSPI SPI1(VARIANT_SPI1_SDI, VARIANT_SPI1_SDO, VARIANT_SPI1_CLK);
#endif
#if VARIANT_SPI_INTFCS > 2
arduino::MbedSPI SPI2(VARIANT_SPI2_SDI, VARIANT_SPI2_SDO, VARIANT_SPI2_CLK);
#endif
#if VARIANT_SPI_INTFCS > 3
arduino::MbedSPI SPI3(VARIANT_SPI3_SDI, VARIANT_SPI3_SDO, VARIANT_SPI3_CLK);
#endif
#if VARIANT_SPI_INTFCS > 4
arduino::MbedSPI SPI4(VARIANT_SPI4_SDI, VARIANT_SPI4_SDO, VARIANT_SPI4_CLK);
#endif
#if VARIANT_SPI_INTFCS > 5
arduino::MbedSPI SPI5(VARIANT_SPI5_SDI, VARIANT_SPI5_SDO, VARIANT_SPI5_CLK);
#endif
