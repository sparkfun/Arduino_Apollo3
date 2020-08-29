/*
  Author: Nathan Seidle
  SparkFun Electronics
  Created: August 18th, 2019

  Purchasing from SparkFun helps write code like this and helps us
  release products open source so that we can help each other learn: https://www.sparkfun.com/artemis

  Arduino defaults to 8-pin PWM resolution. This works for the majority of servos but can
  be increased for projects that need it.

  servoWriteResolution()
  getServoResolution()

  Are provided for changing and reading the resolution setting.

  Hardware Connections:
  Load this code
  Connect a Servo to pin 18:
    Red Wire -> 3.3V or 5V
    Black Wire -> GND
    Signal (Yellow or White) -> 8

  The servo will rotate back and forth.
*/

/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include <Servo.h>

Servo myServoA;

void setup()
{
  Serial.begin(9600);
  Serial.println("SparkFun Servo Example");

  myServoA.attach(8);

  servoWriteResolution(10); //Increase to 10-bit resolution. 16-bit is max.

  int currentRes = getServoResolution();
  Serial.print("Current resolution: ");
  Serial.println(currentRes);
}

void loop()
{
  myServoA.write(180); //Go to max position
  delay(2000);
  myServoA.write(0); //Go to min position
  delay(2000);
}
