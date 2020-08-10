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

#ifndef _APOLLO3_LIBRARIES_WIRE_H_
#define _APOLLO3_LIBRARIES_WIRE_H_

#include "Arduino.h"
#include "Print.h"
#include "drivers/I2C.h"
#include "drivers/I2CSlave.h"
#include "rtos.h"

typedef void (*voidFuncPtrParamInt)(int);

namespace arduino {

class MbedI2C : public HardwareI2C
{
  public:
    MbedI2C(int sda, int scl);
    virtual void begin();
    virtual void begin(uint8_t address);
    virtual void end();

    virtual void setClock(uint32_t freq);
  
    virtual void beginTransmission(uint8_t address);
    virtual uint8_t endTransmission(bool stopBit);
    virtual uint8_t endTransmission(void);

    virtual uint8_t requestFrom(uint8_t address, size_t len, bool stopBit);
    virtual uint8_t requestFrom(uint8_t address, size_t len);

    virtual void onReceive(void(*)(int));
    virtual void onRequest(void(*)(void));

    virtual size_t write(uint8_t data);
    virtual size_t write(const uint8_t* data, int len);
    using Print::write;
    virtual int read();
    virtual int peek();
    virtual void flush();
    virtual int available();

private:

#ifdef DEVICE_I2CSLAVE
    mbed::I2CSlave* slave;
#endif
    mbed::I2C*      master;
    int _sda;
    int _scl;
    int _address;
    RingBufferN<256> rxBuffer;
    uint8_t txBuffer[256];
    uint32_t usedTxBuffer;
    voidFuncPtrParamInt onReceiveCb = NULL;
    voidFuncPtr onRequestCb = NULL;
    rtos::Thread slave_th;
    void receiveThd();
};

}

typedef arduino::MbedI2C TwoWire;

#if VARIANT_WIRE_INTFCS > 0
extern arduino::MbedI2C Wire;
#endif
#if VARIANT_WIRE_INTFCS > 1
extern arduino::MbedI2C Wire1;
#endif
#if VARIANT_WIRE_INTFCS > 2
extern arduino::MbedI2C Wire2;
#endif
#if VARIANT_WIRE_INTFCS > 3
extern arduino::MbedI2C Wire3;
#endif
#if VARIANT_WIRE_INTFCS > 4
extern arduino::MbedI2C Wire4;
#endif
#if VARIANT_WIRE_INTFCS > 5
extern arduino::MbedI2C Wire5;
#endif

#endif // _APOLLO3_LIBRARIES_WIRE_H_
