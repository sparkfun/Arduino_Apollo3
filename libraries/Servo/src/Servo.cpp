/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "Servo.h"

Servo::Servo()
{
}

void Servo::attach(uint8_t pinNumber, uint16_t minMicros, uint16_t maxMicros) {
	_servoPinNumber = pinNumber;
	_minMicros = minMicros;
	_maxMicros = maxMicros;
	pinMode(_servoPinNumber, OUTPUT);
}

void Servo::attach(uint8_t pinNumber) {
	attach(pinNumber, 544, 2400); //Start with Arduino defaults
}

void Servo::write(uint8_t servoPosition) {
	_servoPosition = servoPosition;
	if (_servoPosition > 180)
		_servoPosition = 180; //Bounds check

	uint16_t newServoPosition = map(servoPosition, 0, 181, 0, ((uint16_t)0x01 << getServoResolution()));
	servoWrite(_servoPinNumber, newServoPosition, _minMicros, _maxMicros);
}

void Servo::writeMicroseconds(uint16_t microSecs) {
	uint16_t extendedMin = _minMicros;
	uint16_t extendedMax = _maxMicros;

	if (microSecs > _maxMicros)
		extendedMax = microSecs;
	if (microSecs < _minMicros)
		extendedMin = microSecs;

	//Map microseconds to PWM value
	uint16_t newServoPosition = map(microSecs, extendedMin, extendedMax + 1, 0, ((uint16_t)0x01 << getServoResolution()));
	servoWrite(_servoPinNumber, newServoPosition, extendedMin, extendedMax);
}

void Servo::detach(void) {
	pinMode(_servoPinNumber, INPUT); //Will stop PWM output
	_servoPinNumber = 0;
}

uint8_t Servo::read(void) {
	return (_servoPosition);
}

bool Servo::attached(uint8_t pinNumber){
	if (_servoPinNumber == pinNumber)
		return (true);
	return (false);
}
