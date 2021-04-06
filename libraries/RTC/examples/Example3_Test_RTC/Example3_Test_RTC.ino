/*
  Author: Nathan Seidle and stephenf7072
  Created: January 28th, 2020

  This example test the internal HAL to make sure the days advance correctly.
*/

/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "RTC.h"

int previousDay;

void setup()
{
  Serial.begin(115200);
  delay(10);
  Serial.println("Artemis RTC Testing");

  // Manually set RTC date and time to the start of 2020 so that RTC contains valid times
  rtc.setTime(0, 59, 59, 23, 1, 1, 20); // Set to 1 second before midnight Jan 1
}

void loop()
{
  rtc.setTime(99, 59, 59, 23, rtc.dayOfMonth, rtc.month, rtc.year); // Manually set RTC 1/100th of a second from the next day
  previousDay = rtc.weekday;
  delay(11); //Allow us to roll from midnight the night before to the new day

  printArtemisTime();
}

void printArtemisTime()
{
  char buf[50];
  char weekdayBuf[4];

  rtc.getTime();
  int i = rtc.weekday + 1;
  switch (i)
  {
  case (1):
    strcpy(weekdayBuf, "Sun");
    break;
  case (2):
    strcpy(weekdayBuf, "Mon");
    break;
  case (3):
    strcpy(weekdayBuf, "Tue");
    break;
  case (4):
    strcpy(weekdayBuf, "Wed");
    break;
  case (5):
    strcpy(weekdayBuf, "Thu");
    break;
  case (6):
    strcpy(weekdayBuf, "Fri");
    break;
  case (7):
    strcpy(weekdayBuf, "Sat");
    break;

  default:
    strcpy(weekdayBuf, "???");
    break;
  }

  sprintf(buf, "%02d-%02d-%02d (%s) %02d:%02d:%02d.%02d", rtc.year, rtc.month, rtc.dayOfMonth, weekdayBuf, rtc.hour, rtc.minute, rtc.seconds, rtc.hundredths);
  Serial.print(buf);

  //Move the previous day forward one day and make sure it matches today
  if ((previousDay + 1) % 7 != rtc.weekday)
  {
    Serial.printf(" Error! previousDay: %d today: %d\n", previousDay, rtc.weekday);
    while (1){};
  }

  Serial.println();
}
