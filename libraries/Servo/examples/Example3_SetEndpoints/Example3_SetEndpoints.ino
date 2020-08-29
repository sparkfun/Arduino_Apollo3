/*
  Author: Nathan Seidle
  SparkFun Electronics
  Created: August 18th, 2019

  Purchasing from SparkFun helps write code like this and helps us
  release products open source so that we can help each other learn: https://www.sparkfun.com/artemis

  Different servos have different end points. Arduino defaults to 544us (min) and 2400us (max).
  These however can be changed at startup. For more info see: https://www.arduino.cc/en/Reference/ServoAttach

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

  myServoA.attach(8, 500, 2600); //Increase min/max to drive servo fully
}

void loop()
{
  myServoA.write(180); //Go to max position
  delay(2000);
  myServoA.write(0); //Go to min position
  delay(2000);
}
