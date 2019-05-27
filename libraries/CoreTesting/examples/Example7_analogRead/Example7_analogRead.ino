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

  //TODO reset analog configuration status back to false when user changes pin back to GPIO
  //pinMode()
  //Research way to tell what config the pin is in

  //There is internal temp sensor channel 0xC, and Batt 0xD

  //analogReadResolution(14); //Set resolution to 14 bit
  //analogReadResolution(16); //Set resolution to 16 bit - will pad ADC output with two zeros
}

void loop() {
  digitalWrite(LED, LOW);

  int myValue1 = analogRead(34); //Read pad 34, ADC channel 6
  Serial.print(" left val: ");
  Serial.print(myValue1);

  int myValue2 = analogRead(16);
  Serial.print(" right val: ");
  Serial.print(myValue2);

  int internalTemp = analogRead(ADC_TEMP); //Read internal temp sensor
  Serial.print(" internalTemp: ");
  Serial.print(internalTemp);

  int vss = analogRead(ADC_VSS); //Read internal VSS
  Serial.print(" vss: ");
  Serial.print(vss);

  //TODO enable battload
  int div3 = analogRead(ADC_DIV3); //Read VCC across a 1/3 resistor divider
  Serial.print(" VCC/3: ");
  Serial.print(div3);

  //Todo: test overall read time
  /*long startTime = millis();
  uint32_t total = 0;
  for(int x = 0 ; x < 100 ; x++)
  {
    total += analogRead(16);
  }
  Serial.print(" total: ");
  Serial.print(total);
  Serial.print(" totalTime: ");
  Serial.print( (millis() - startTime) / 100);*/
 
  Serial.println();

  digitalWrite(LED, HIGH);
  delay(5);
}
