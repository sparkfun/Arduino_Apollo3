/*
  Author: Nathan Seidle
  SparkFun Electronics
  Created: August 18th, 2019

  Purchasing from SparkFun helps write code like this and helps us
  release products open source so that we can help each other learn: https://www.sparkfun.com/artemis

  This example demonstrates the control of 29 servos simulataneously on the BlackBoard Artemis ATP.
  Any PWM pin is capable of 16 bit servo control. Need more than 29 servos? What are you building?!

  Hardware Connections:
  Load this code
  Connect a Servo to pin 18:
    Red Wire -> 3.3V or 5V
    Black Wire -> GND
    Signal (Yellow or White) -> 18

  The servo will rotate back and forth. Similary, you can move the signal wire to any PWM
  pin and you will see the servo move to a new position and hold. This shows that the other
  PWM pins are all simultaneously and actively outputting a servo signal.

  There are 30 PWM capable pads on the Artemis. Pin 47 is used for bootloading and is not
  easily accessible. There are 29 easily accessible on the ATP.
*/

/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include <Servo.h>

Servo myServoSCL;
Servo myServoSCK;
Servo myServoMISO;
Servo myServoMOSI;
Servo myServo13;
Servo myServo12;
Servo myServo32;
Servo myServo28;
Servo myServo27;
Servo myServo23;

Servo myServo22;
Servo myServo4;
Servo myServo35;
Servo myServoTX1;
Servo myServoRX1;
Servo myServo26;
Servo myServo19;
Servo myServo18;
Servo myServo37;
Servo myServo44;

Servo myServo42;
Servo myServo43;
Servo myServo45;
Servo myServo31;
Servo myServo33;
Servo myServo11;
Servo myServo29;
Servo myServo48;
Servo myServo49;

void setup()
{
  Serial.begin(9600);
  Serial.println("SparkFun Servo Example");

  myServoSCL.attach(SCL);
  myServoSCK.attach(5); //Labeled SCK
  myServoMISO.attach(6); //Labeled MISO
  myServoMOSI.attach(7); //Labeled MOSI
  myServo13.attach(13);
  myServo12.attach(12);
  myServo32.attach(32);
  myServo28.attach(28);
  myServo27.attach(27);
  myServo23.attach(23);
  myServo22.attach(22);
  myServo4.attach(4);
  myServo35.attach(35);
  myServoTX1.attach(24); //Labeled TX1
  myServoRX1.attach(25); //Labeled RX1
  myServo26.attach(26);
  myServo19.attach(19);
  myServo18.attach(18);
  myServo37.attach(37);
  myServo44.attach(44);
  myServo42.attach(42);
  myServo43.attach(43);
  myServo45.attach(45);
  myServo31.attach(31);
  myServo33.attach(33);
  myServo11.attach(11);
  myServo29.attach(29);
  myServo48.attach(48); //Labeled TX0 (used for bootloading)
  myServo49.attach(49); //Labeled RX0 (used for bootloading)

  int startAmount = 600;

  myServoSCL.writeMicroseconds(startAmount + 0); //600
  myServoSCK.writeMicroseconds(startAmount + 50); //650
  myServoMISO.writeMicroseconds(startAmount + 100); //700
  myServoMOSI.writeMicroseconds(startAmount + 150); //750
  myServo13.writeMicroseconds(startAmount + 200); //800
  myServo12.writeMicroseconds(startAmount + 250); //850
  myServo32.writeMicroseconds(startAmount + 300); //900
  myServo28.writeMicroseconds(startAmount + 350); //950
  myServo27.writeMicroseconds(startAmount + 400); //1000
  myServo23.writeMicroseconds(startAmount + 450); //1050
  myServo22.writeMicroseconds(startAmount + 500); //1100
  myServo4.writeMicroseconds(startAmount + 550); //1150
  myServo35.writeMicroseconds(startAmount + 600); //1200
  myServoTX1.writeMicroseconds(startAmount + 650); //1250
  myServoRX1.writeMicroseconds(startAmount + 700); //1300
  myServo26.writeMicroseconds(startAmount + 750); //1350
  myServo19.writeMicroseconds(startAmount + 800); //1400
  myServo18.writeMicroseconds(startAmount + 850); //1450
  myServo37.writeMicroseconds(startAmount + 900); //1500
  myServo44.writeMicroseconds(startAmount + 950); //1550
  myServo42.writeMicroseconds(startAmount + 1000); //1600
  myServo43.writeMicroseconds(startAmount + 1050); //1650
  myServo45.writeMicroseconds(startAmount + 1100); //1700
  myServo31.writeMicroseconds(startAmount + 1150); //1750
  myServo33.writeMicroseconds(startAmount + 1200); //1800
  myServo11.writeMicroseconds(startAmount + 1250); //1850
  myServo29.writeMicroseconds(startAmount + 1300); //1900
  myServo48.writeMicroseconds(startAmount + 1350); //1950
  myServo49.writeMicroseconds(startAmount + 1400); //2000
}

void loop()
{
  myServo18.write(0);
  delay(1500);
  myServo18.write(180);
  delay(1500);
}
