/*
  Author: Nathan Seidle
  SparkFun Electronics
  Created: June 19 2019
  License: MIT. See SparkFun Arduino Apollo3 Project for more information

  Feel like supporting open source hardware? Buy a board from SparkFun!
  https://www.sparkfun.com/artemis

  This example shows how to send characters at 9600bps. Any pin can be used for
  TX or RX.

  Note: In this SoftwareSerial library you cannot TX and RX at the same time.
  TX gets priority. So if Artemis is receiving a string of characters
  and you do a Serial.print() the print will begin immediately and any additional
  RX characters will be lost.  

  Hardware Connections:
  Attach a USB to serial converter (https://www.sparkfun.com/products/15096)
  Connect
    GND on SerialBasic <-> GND on Artemis
    RXO on SerialBasic <-> Pin 8 on Artemis
    TXO on SerialBasic <-> Pin 7 on Artemis
  Load this code
  Open Arduino serial monitor at 57600
  Open Terminal window (TeraTerm) at 9600
  Press a button in terminal window, you should see it in Arduino monitor
*/

#include <SoftwareSerial.h>
SoftwareSerial mySerial(7, 8); //RX, TX - Any pins can be used

int counter = 0;

void setup() {
  //We set the serial monitor speed high so that we spend less time printing the output
  //and more time checking mySerial.available()
  Serial.begin(57600); 
  Serial.println("Software Serial Example");

  mySerial.begin(9600);
}

void loop() {

  mySerial.print("Hi: ");
  mySerial.println(counter++);
  
  while (mySerial.available())
  {
    byte incoming = mySerial.read();
    Serial.write(incoming);
  }

  delay(100); //Small delay between prints so that we can detect incoming chars if any
}
