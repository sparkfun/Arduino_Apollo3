/*
  Author: Nathan Seidle
  Created: Septempter 27th, 2019

  This example demonstrates how to initialize and read from the on-board RTC.

  Most SparkFun Artemis boards have the necessary external 32kHz crystal to
  enable the RTC. If you are using the Artemis module bare you will either
  need an external 32kHz xtal or use the internal LFRC. Read the datasheet
  section 12.1 for more information.

  This example is based on the Ambiq SDK EVB2 RTC example.
*/

/*
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "RTC.h" // Include RTC library included with the Aruino_Apollo3 core

void setup()
{
  Serial.begin(115200);
  Serial.println("SparkFun RTC Example");

  // Easily set RTC using the system __DATE__ and __TIME__ macros from compiler
  rtc.setToCompilerTime();

  // Manually set RTC date and time
  // rtc.setTime(0, 50, 59, 12, 3, 6, 20); // 12:59:50.000, June 3rd, 2020 (hund, ss, mm, hh, dd, mm, yy)
}

void loop()
{
  rtc.getTime();

  Serial.printf("It is now ");
  Serial.printf("%d:", rtc.hour);
  Serial.printf("%02d:", rtc.minute);
  Serial.printf("%02d.", rtc.seconds);
  Serial.printf("%02d", rtc.hundredths);

  Serial.printf(" %02d/", rtc.month);
  Serial.printf("%02d/", rtc.dayOfMonth);
  Serial.printf("%02d", rtc.year);

  Serial.printf(" Day of week: %d =", rtc.weekday);
  Serial.printf(" %s", rtc.textWeekday);

  Serial.println();

  delay(1000);
}
