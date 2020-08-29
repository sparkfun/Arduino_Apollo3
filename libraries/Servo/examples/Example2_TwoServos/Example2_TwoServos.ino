/*
  Author: Nathan Seidle
  SparkFun Electronics
  Created: August 18th, 2019

  Purchasing from SparkFun helps write code like this and helps us
  release products open source so that we can help each other learn: https://www.sparkfun.com/artemis

  This example demonstrates controlling two servos.

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
Servo myServoB;

int pos = 0;

void setup()
{
  Serial.begin(9600);
  Serial.println("SparkFun Servo Example");

  myServoA.attach(8);
  myServoB.attach(7);
}

void loop()
{
  //Sweep
  for (pos = 0; pos <= 180; pos++) {
    myServoA.write(pos);
    myServoB.write(180 - pos);
    delay(5);
  }
  for (pos = 180; pos >= 0; pos--) {
    myServoA.write(pos);
    myServoB.write(180 - pos);
    delay(5);
  }
}
