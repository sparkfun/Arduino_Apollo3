/*

Copyright (c) 2015 Arduino LLC. All rights reserved.
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

#include "Servo.h"
#include "Arduino.h"

//Constructor
Servo::Servo()
{
}

//Assign global var
void Servo::attach(uint8_t pinNumber)
{
	_servoPinNumber = pinNumber;
	pinMode(_servoPinNumber, OUTPUT);
}

void Servo::write(uint8_t servoPosition)
{
	_servoPosition = servoPosition;
	if (_servoPosition > 180)
		_servoPosition = 180; //Bounds check

	uint16_t newServoPosition = map(servoPosition, 0, 181, 0, ((uint16_t)0x01 << getServoResolution()));

	servoWrite(_servoPinNumber, newServoPosition); // This and the above write should both produce 1.5 ms wide pulses, though using different resolutions
}

// void Servo::writeMicroseconds(uint8_t servoPosition)
// {
// 	servoWrite(_servoPadNumber, newServoPosition); // This and the above write should both produce 1.5 ms wide pulses, though using different resolutions
// }

void Servo::detach(void)
{
	pinMode(_servoPinNumber, INPUT); //Will stop PWM output

	_servoPinNumber = 0;
}

uint8_t Servo::read(void)
{
	return (_servoPosition);
}

bool Servo::attached(uint8_t pinNumber)
{
	if (_servoPinNumber == pinNumber)
		return (true);
	return (false);
}