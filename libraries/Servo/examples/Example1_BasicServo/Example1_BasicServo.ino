/*
  Author: Nathan Seidle
  SparkFun Electronics
  Created: August 18th, 2019

  Purchasing from SparkFun helps write code like this and helps us
  release products open source so that we can help each other learn: https://www.sparkfun.com/artemis

  This example demonstrates the control of a servo on pin 8 on the RedBoard Artemis. Any PWM
  pin can control a servo.

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

Servo myServo;

int pos = 0;

void setup()
{
  Serial.begin(9600);
  Serial.println("SparkFun Servo Example");

  myServo.attach(8);
}

void loop()
{
  //Sweep
  for (pos = 0; pos <= 180; pos++) { // goes from 0 degrees to 180 degrees
    myServo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos--) { // goes from 180 degrees to 0 degrees
    myServo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15ms for the servo to reach the position
  }
}
