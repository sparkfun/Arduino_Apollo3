/*
  Analog to digital conversion
  By: Nathan Seidle
  SparkFun Electronics
  Date: May 2nd, 2019
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This example shows how to read an analog voltage from various ADC enabled pins.

  Some boards have A0, A1, etc. Some boards have A23, A35, etc. Be sure to
  select the correct board from the boards menu and modify the code to match those pins.

  Feel like supporting open source hardware?
  Buy a board from SparkFun!
  SparkFun Edge: https://www.sparkfun.com/artemis

  Hardware Connections:
  Upload code
  Use a trimpot or other device to send a 0 to 3.3V (no 5V!) signal to pin 'A16'
  ADC can resolve up to 2V but is 3.3V safe
*/

#define LED LED_BUILTIN //Status LED on Artemis carrier boards

void setup()
{
  Serial.begin(9600);
  Serial.println("SparkFun Arduino Apollo3 Analog Read example");

  pinMode(LED, OUTPUT);

  analogReadResolution(14); //Set resolution to 14 bit
  //analogReadResolution(16); //Set resolution to 16 bit - will pad ADC output with two zeros
}

void loop()
{
  digitalWrite(LED, LOW);

  int myValue1 = analogRead(A3); //Automatically sets pin to analog input
  Serial.print("A3: ");
  Serial.print(myValue1);

  //TODO enable battload
  int div3 = analogRead(ADC_INTERNAL_VCC_DIV3); //Read VCC across a 1/3 resistor divider
  Serial.print("\tVCC/3: ");
  Serial.print(div3);

  float vcc = (float)div3 * 6 / 16384.0; //Convert 1/3 VCC to VCC
  Serial.print(" VCC: ");
  Serial.print(vcc, 2);
  Serial.print("V");

  Serial.print("\tinternalTemp: ");
  Serial.print(getInternalTemp(), 2);

  int vss = analogRead(ADC_INTERNAL_VSS); //Read internal VSS (should be 0)
  Serial.print("\tvss: ");
  Serial.print(vss);

  Serial.println();

  digitalWrite(LED, HIGH);
  delay(5);
}
