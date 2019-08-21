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

#ifndef _AP3_SERVO_H_
#define _AP3_SERVO_H_

#include "Arduino.h"

class Servo
{
public:
  Servo();
  void attach(uint8_t pinNumber, uint16_t minMicros, uint16_t maxMicros);
  void attach(uint8_t pinNumber);
  void write(uint8_t servoPosition);
  void writeMicroseconds(uint16_t microSecs);
  void detach(void);
  uint8_t read(void);
  bool attached(uint8_t pinNumber);

private:
  uint8_t _servoPinNumber = 0;
  uint8_t _servoPosition;
  uint16_t _minMicros;
  uint16_t _maxMicros;
};

#endif // _AP3_SERVO_H_
