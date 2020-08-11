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
  RTC.setTime(0, 59, 59, 23, 1, 1, 20); // Set to 1 second before midnight Jan 1
}

void loop()
{
  RTC.setTime(99, 59, 59, 23, RTC.dayOfMonth, RTC.month, RTC.year); // Manually set RTC 1/100th of a second from the next day
  previousDay = RTC.weekday;
  delay(11); //Allow us to roll from midnight the night before to the new day

  printArtemisTime();
}

void printArtemisTime()
{
  char buf[50];
  char weekdayBuf[4];

  RTC.getTime();
  int i = RTC.weekday + 1;
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

  sprintf(buf, "%02d-%02d-%02d (%s) %02d:%02d:%02d.%02d", RTC.year, RTC.month, RTC.dayOfMonth, weekdayBuf, RTC.hour, RTC.minute, RTC.seconds, RTC.hundredths);
  Serial.print(buf);

  //Move the previous day forward one day and make sure it matches today
  if ((previousDay + 1) % 7 != RTC.weekday)
  {
    Serial.printf(" Error! previousDay: %d today: %d\n", previousDay, RTC.weekday);
    while (1){};
  }

  Serial.println();
}