/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#ifndef _APOLLO3_LIBRARIES_SERVO_H_
#define _APOLLO3_LIBRARIES_SERVO_H_

#include "Arduino.h"

class Servo {
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

#endif // _APOLLO3_LIBRARIES_SERVO_H_
