/*   
  Author: Nathan Seidle
  SparkFun Electronics
  Created: August 18th, 2019

  Purchasing from SparkFun helps write code like this and helps us
  release products open source so that we can help each other learn: https://www.sparkfun.com/artemis

  This example demonstrates the control of 19 servos simulataneously on the BlackBoard Artemis.
  Any PWM pin is capable of 16 bit servo control. Need more than 19 servos? What are you building?!
  Check out the BlackBoard ATP for 29 servo capable PWM pins.

  Hardware Connections:
  Load this code
  Connect a Servo to RX1:
    Red Wire -> 3.3V or 5V
    Black Wire -> GND
    Signal (Yellow or White) -> RX1

  The servo will rotate back and forth. Similary, you can move the signal wire to any PWM
  pin and you will see the servo move to a new position and hold. This shows that the other
  PWM pins are all simultaneously and actively outputting a servo signal.
*/

/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include <Servo.h>

Servo myServoRX;
Servo myServoTX;
Servo myServo2;
Servo myServo3;
Servo myServo4;
Servo myServo5;
Servo myServo6;
Servo myServo7;
Servo myServo8;
Servo myServo9;
Servo myServo10;
Servo myServoMOSI;
Servo myServoMISO;
Servo myServoSCK;
Servo myServoA0;
Servo myServoA1;
Servo myServoA3;
Servo myServoA5;
Servo myServoSCL;

int pos = 0;

void setup()
{
  Serial.begin(9600);
  Serial.println("SparkFun Servo Example");

  myServoRX.attach(0); //Labeled RX1
  myServoTX.attach(1); //Labeled TX1
  myServo2.attach(2);
  myServo3.attach(3);
  myServo4.attach(4);
  myServo5.attach(5);
  myServo6.attach(6);
  myServo7.attach(7);
  myServo8.attach(8);
  myServo9.attach(9);
  myServo10.attach(10);
  myServoMOSI.attach(11); //Labeled MOSI
  myServoMISO.attach(12); //Labeled MISO
  myServoSCK.attach(13); //Labeled SCK
  myServoA0.attach(A0);
  myServoA1.attach(A1);
  myServoA3.attach(A3);
  myServoA5.attach(A5);
  myServoSCL.attach(SCL);

  myServo2.write(0);
  myServo3.write(10);
  myServo4.write(30);
  myServo5.write(60);
  myServo6.write(90);
  myServo7.write(120);
  myServo8.write(140);
  myServo9.write(160);
  myServo10.write(180);
  myServoMOSI.write(0);
  myServoMISO.write(30);
  myServoSCK.write(60);
  myServoA0.write(90);
  myServoA1.write(120);
  myServoA3.write(150);
  myServoA5.write(0);
  myServoSCL.write(30);
  myServoTX.write(60);
}

void loop()
{
  myServoRX.write(0);
  delay(1500);
  myServoRX.write(180);
  delay(1500);
}
