/*
  Analog to digital conversion
  By: Nathan Seidle
  SparkFun Electronics
  Date: May 2nd, 2019
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This example shows how to read an analog voltage from various ADC enabled pins.

  Feel like supporting open source hardware?
  Buy a board from SparkFun!
  SparkFun Edge: https://www.sparkfun.com/products/15170

  Hardware Connections:
  Connect an Edge via programmer
  Upload code
  Use a trimpot or other device to send a 0 to 3V (no 5V!) signal to pin 'A16'
  Max reading on ADC is 2V
*/

#include <ap3_analog.h>

#define LED 5 //Status LED

//Incase user uses the original Arduino style of pin referencing
#define A16 16
#define A29 29

void setup() {
  Serial.begin(9600);
  Serial.println("SparkFun Arduino Apollo3 Analog Read example");

  delay(100);

  pinMode(LED, OUTPUT);

  //What is best way to prevent user from doing
  //analogRead(not an analog pin); ?
  //Force users to use A31 nomenclature?
  //Allow but return 0 or some error
  //

  //There are 10 analog channels: SE0 to SE9 and map uncleanly to adc=pin, 2=11, 1=29, etc
  //Do we want to label boards A29 or A1? I think A29. Continue the direct map to pin nomenclature.

  //TODO reset analog configuration status back to false when user changes pin back to GPIO
  //pinMode()
  //Research way to tell what config the pin is in

  //There is internal temp sensor channel 0xC, and Batt 0xD

  //analogReadResolution(14); //Set resolution to 14 bit
  //analogReadResolution(16); //Set resolution to 16 bit - will pad ADC output with two zeros

  ap3_adc_setup();
}

void loop() {
  digitalWrite(LED, LOW);

  int myValue1 = analogRead(34);
  Serial.print(" left val: ");
  Serial.print(myValue1);

  int myValue2 = analogRead(16);
  Serial.print(" right val: ");
  Serial.print(myValue2);

  Serial.println();

  digitalWrite(LED, HIGH);
  delay(5);
}
