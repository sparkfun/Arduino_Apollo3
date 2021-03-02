/*
  Author: Adam Garbo and Nathan Seidle
  Created: June 3rdrd, 2020

  This example demonstrates how to read and set rolling RTC alarms. Each time
  the alarm triggers, a user-specified additional amount of time (seconds,
  minutes or hours) can be added to create a rolling RTC alarm. Second,
  minute and hour rollovers are handled using modulo calculations.

  The current code is configured as a 5-second rolling RTC alarm.
*/

/*
  // This file is subject to the terms and conditions defined in
  // file 'LICENSE.md', which is part of this source code package.
*/

#include "RTC.h"

volatile bool alarmFlag = false;
int alarmSeconds = 5;
int alarmMinutes = 0;
int alarmHours = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println("SparkFun RTC Set Rolling Alarms Example");

  // // Easily set RTC using the system __DATE__ and __TIME__ macros from compiler
  // RTC.setToCompilerTime();

  // Manually set RTC date and time
  rtc.setTime(0, 50, 59, 12, 3, 6, 20); // 12:59:50.000, June 3rd, 2020 (hund, ss, mm, hh, dd, mm, yy)

  // Set the RTC's alarm
  rtc.setAlarm(0, 0, 0, 13, 3, 6); // 13:00:00.000, June 3rd (hund, ss, mm, hh, dd, mm). Note: No year alarm register

  // Set the RTC alarm mode
  /*
    0: Alarm interrupt disabled
    1: Alarm match every year   (hundredths, seconds, minutes, hour, day, month)
    2: Alarm match every month  (hundredths, seconds, minutes, hours, day)
    3: Alarm match every week   (hundredths, seconds, minutes, hours, weekday)
    4: Alarm match every day    (hundredths, seconds, minute, hours)
    5: Alarm match every hour   (hundredths, seconds, minutes)
    6: Alarm match every minute (hundredths, seconds)
    7: Alarm match every second (hundredths)
  */
  rtc.setAlarmMode(6); // Set the RTC alarm to match on minutes rollover
  rtc.attachInterrupt(); // Attach RTC alarm interrupt

  // Print the RTC's alarm date and time
  Serial.print("Next alarm: "); printAlarm();
}

void loop()
{
  // Check if alarm flag was set
  if (alarmFlag == true)
  {
    // Print date and time of RTC alarm trigger
    Serial.print("Alarm triggered: "); printDateTime();

    // Clear alarm flag
    alarmFlag = false;

    // Set the RTC's rolling alarm
    rtc.setAlarm(0,
                 (rtc.seconds + alarmSeconds) % 60,
                 (rtc.minute + alarmMinutes) % 60,
                 (rtc.hour + alarmHours) % 24,
                 rtc.dayOfMonth,
                 rtc.month);
    rtc.setAlarmMode(6);

    // Print next RTC alarm date and time
    Serial.print("Next rolling alarm: "); printAlarm();
  }
}

// Print the RTC's current date and time
void printDateTime()
{
  rtc.getTime();
  Serial.printf("20%02d-%02d-%02d %02d:%02d:%02d.%03d\n",
                rtc.year, rtc.month, rtc.dayOfMonth,
                rtc.hour, rtc.minute, rtc.seconds, rtc.hundredths);
  Serial.println();
}

// Print the RTC's alarm
void printAlarm()
{
  rtc.getAlarm();
  Serial.printf("2020-%02d-%02d %02d:%02d:%02d.%03d\n",
                rtc.alarmMonth, rtc.alarmDayOfMonth,
                rtc.alarmHour, rtc.alarmMinute,
                rtc.alarmSeconds, rtc.alarmHundredths);
  Serial.println();
}

// Interrupt handler for the RTC
extern "C" void am_rtc_isr(void)
{
  // Clear the RTC alarm interrupt.
  rtc.clearInterrupt();

  // Set alarm flag
  alarmFlag = true;
}
