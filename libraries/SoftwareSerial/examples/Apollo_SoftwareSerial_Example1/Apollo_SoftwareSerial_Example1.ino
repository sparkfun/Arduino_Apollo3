/*
  Author: Nathan Seidle
  SparkFun Electronics
  Created: June 19 2019
  License: MIT. See SparkFun Arduino Apollo3 Project for more information

  Feel like supporting open source hardware? Buy a board from SparkFun!
  https://www.sparkfun.com/artemis

  This example shows how to receive characters at 9600bps. Any pin can be used for 
  reception.

  Hardware Connections:
  Attach a USB to serial converter (https://www.sparkfun.com/products/15096)
  Connect 
    GND on SerialBasic <-> GND on Artemis
    TXO on SerialBasic <-> Pin 7 on Artemis
  Load this code
  Open Arduino serial monitor at 9600
  Open Terminal window (TeraTerm) at 9600
  Press a button in terminal window, you should see it in Arduino monitor  
*/


//#define DEBUG
#define LED 13

#include <SoftwareSerial.h>
SoftwareSerial mySerial(7, 8); //RX, TX

void setup() {
  Serial.begin(9600);
  Serial.println("Software Serial Example");

  //enableBurstMode(); //Go to 96MHz

  mySerial.begin(9600); //SERIAL_8N1); 
}

void loop() {
  digitalWrite(LED, HIGH);
  //delay(100);
  digitalWrite(LED, LOW);
  //delay(100);

  delay(1);


}
