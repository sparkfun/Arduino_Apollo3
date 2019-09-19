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

// ToDo: Make TwoWire a library (under "libraries") to get syntax highlighting

#ifndef _AP3_WIRE_H_
#define _AP3_WIRE_H_

#include "Arduino.h"
#include "ap3_iomaster.h"

#define AP3_WIRE_RX_BUFFER_LEN 256
#define AP3_WIRE_TX_BUFFER_LEN 256
#define AP3_WIRE_LINEAR_BUFFER_LEN (AP3_WIRE_RX_BUFFER_LEN+AP3_WIRE_TX_BUFFER_LEN)

 // WIRE_HAS_END means Wire has end()
#define WIRE_HAS_END 1

class TwoWire : public Stream, public IOMaster {
  public:
    TwoWire(uint8_t iom_instance);
    void begin();
    void begin(uint8_t, bool enableGeneralCall = false);
    void end();
    void setClock(uint32_t);

    void beginTransmission(uint8_t address);
    uint8_t endTransmission(bool stopBit = true);

    uint8_t requestFrom(uint8_t address, size_t quantity, bool stopBit = true);

    size_t write(uint8_t data);
    size_t write(const uint8_t * data, size_t quantity);

    virtual int available(void);
    virtual int read(void);
    virtual int peek(void);
    virtual void flush(void);
    void onReceive(void(*)(int));
    void onRequest(void(*)(void));

    inline size_t write(unsigned long n) { return write((uint8_t)n); }
    inline size_t write(long n) { return write((uint8_t)n); }
    inline size_t write(unsigned int n) { return write((uint8_t)n); }
    inline size_t write(int n) { return write((uint8_t)n); }
    using Print::write;

    void onService(void);

  private:
    ap3_gpio_pin_t  _padSDA;
    ap3_gpio_pin_t  _padSCL;

    bool _transmissionBegun;
    uint8_t _transmissionAddress;

    
    RingBufferN<AP3_WIRE_RX_BUFFER_LEN> _rxBuffer;// RX Buffer    
    RingBufferN<AP3_WIRE_TX_BUFFER_LEN> _txBuffer;// TX buffer
    uint8_t _linearBugger[AP3_WIRE_LINEAR_BUFFER_LEN]; // ToDo: choose a more efficient way to handle this
    uint8_t txAddress;

    // Callback user functions
    void (*_onRequestCallback)(void);
    void (*_onReceiveCallback)(int);
};

#if WIRE_INTERFACES_COUNT > 0
  extern TwoWire Wire;
#endif
#if WIRE_INTERFACES_COUNT > 1
  extern TwoWire Wire1;
#endif
#if WIRE_INTERFACES_COUNT > 2
  extern TwoWire Wire2;
#endif
#if WIRE_INTERFACES_COUNT > 3
  extern TwoWire Wire3;
#endif
#if WIRE_INTERFACES_COUNT > 4
  extern TwoWire Wire4;
#endif
#if WIRE_INTERFACES_COUNT > 5
  extern TwoWire Wire5;
#endif

#endif // _AP3_WIRE_H_
