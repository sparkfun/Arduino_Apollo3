/*
  Using the differential ADC
  By: Nathan Seidle
  SparkFun Electronics
  Date: July 17th, 2020
  License: This code is public domain. Based on deepsleep.c from Ambiq SDK v2.2.0.

  There are two differential ADC ports on the Artemis. These are located
  on pads 12/13 (DIFF0N/DIFF0P) and 14/15 (DIFF1P/DIFF10).

  A differential ADC port measures -1.0V to 1.0V with
  0 = -1.0V, 8192 = 0V, and 16383 = 1.0V when the ADC is in 14-bit mode.

  SparkFun labored with love to create this code. Feel like supporting open source hardware?
  Buy a board from SparkFun! https://www.sparkfun.com/products/15376
*/

#define ANALOG_RESOLUTION 14 //Artemis has 14 bit ADC but can range from 8 to 14 bit

int maxADCValue = pow(2, ANALOG_RESOLUTION);

void setup()
{
    Serial.begin(115200);
    Serial.println("Differential analog conversion example");

    pinMode(LED_BUILTIN, OUTPUT);

    analogReadResolution(ANALOG_RESOLUTION); //Default is 10 bit. Increase to 14 bit.
}

void loop()
{
    int myAnalog0 = analogRead(ADIFF0); //Pads 12/13. Pins 9/10 on RedBoard Artemis.
    //int myAnalog0 = analogRead(ADIFF1); //Pads 14/15
    float myVoltage0 = mapfloat(myAnalog0, 0, maxADCValue, -1.0, 1.0);
    Serial.print("Diff0 voltage: ");
    Serial.print(myVoltage0, 3);
    Serial.println();

    digitalWrite(LED_BUILTIN, HIGH);
    delay(25);
    digitalWrite(LED_BUILTIN, LOW);
    delay(25);
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
    if (x > in_max)
        x = in_max;

    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}