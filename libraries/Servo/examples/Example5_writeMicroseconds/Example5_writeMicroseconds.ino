/*   
  Author: Nathan Seidle
  SparkFun Electronics
  Created: August 18th, 2019

  Purchasing from SparkFun helps write code like this and helps us
  release products open source so that we can help each other learn: https://www.sparkfun.com/artemis

  This example shows how to write to the servo in microseconds instead of position. This
  is helpful for find the min/max us settings for a given servo manufacturer.

  Hardware Connections:
  Load this code
  Connect a Servo to RX1:
    Red Wire -> 3.3V or 5V
    Black Wire -> GND
    Signal (Yellow or White) -> RX1

  The servo will rotate back and forth. 
*/

/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include <Servo.h>

Servo myServo;

void setup()
{
  Serial.begin(9600);
  Serial.println("SparkFun Servo Example");

  myServo.attach(8);

  myServo.writeMicroseconds(600); //Servo will go to very near the 0 degree position
}

void loop()
{
}
