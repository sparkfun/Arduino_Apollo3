/* Author: Owen Lyke
  Created: June 10 2019
  License: MIT. See SparkFun Arduino Apollo3 Project for more information

  This example demonstrates how to use analogWrite() and associated functions

  Capable Apollo3 pads* are:

    12 25 13 26 18
    27 19 28  5 29
     6 30 22 31 23
    32 42  4 43  7
    44 24 45 33 46
    39 47 35 48 37
    49 11

   these numbers refer to the _pad_ number which does not necessarily mean the _pin_ number of the Apollo3-based variant

  This sketch assumes you are using a "directly mapped" (pin == pad) variant such as the SparkFun Edge board
*/

void setup() {
  Serial.begin(9600);

  while (!Serial) {}; //Wait for user to open terminal window

  Serial.println("SparkFun Arduino Apollo3 analogWrite() Example");
  Serial.printf("Compiled on %s, %s\n\n", __DATE__, __TIME__); // Look! You can use printf, if you so choose

  analogWrite(46, 127);         // analogWrite uses a high frequency counter/timer module to generate a PWM value with f ~= 350 Hz
  analogWriteResolution(15);    // you can increase the resolution of the write from the default (8-bit, 0-255) to up to 15 bits (0-32767)
  analogWrite(37, 16383);       // subsequent cals to analogWrite use the new resolution, so both this call and the original have a 50% duty cycle, or 3.3/2 V

  servoWrite(44, 127);          // The servoWrite function produces a PWM signal with a pulse width between 1 and 2 ms at 500 Hz
  servoWriteResolution(15);     // You can also increase the servoWrite() resolution
  servoWrite(47, 16383);        // This and the above write should both produce 1.5 ms wide pulses, though using different resolutions

  delay(5000);
}

void loop() {
  // All the PWM generation is done in hardware, so the remainder of the code is both simple and efficient!

  // analog and servo write resolutions remain at the last value they were set to
  static int32_t val = 0;
  static int8_t sign = 1;

  analogWrite(46, val);
  analogWrite(37, val);
  servoWrite(44, val);
  servoWrite(47, val);

  val += 5 * sign;
  if (val > 32767) {
    sign = -1;
  }
  if ( val <= 0 ) {
    sign = 1;
  }

  delay(1);
}
