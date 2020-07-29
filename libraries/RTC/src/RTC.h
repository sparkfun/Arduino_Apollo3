/*
Copyright (c) 2020 SparkFun Electronics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef _APOLLO3_LIBRARIES_RTC_H_
#define _APOLLO3_LIBRARIES_RTC_H_

#include <Arduino.h>

#ifdef RTC
#undef RTC
#endif

class Apollo3RTC {
private:
    //Helper functions to convert compiler date/time to ints
    int toVal(char const *pcAsciiStr);
    int mthToIndex(char const *pcMon);

protected:
public:
    Apollo3RTC();

    void getTime(); //Query the RTC for the current time/date
    uint32_t getEpoch(); //Return the current RTC time/date as UNIX Epoch time 

    void setTime(uint8_t hund, uint8_t sec, uint8_t min, uint8_t hour, uint8_t dayOfMonth, uint8_t month, uint16_t year); // Set current time
    void setToCompilerTime(); //Set to time when sketch was compiled
    void setEpoch(uint32_t ts); //Set current time to provided UNIX Epoch time

    void getAlarm(); //Query the RTC for the current alarm time/date
    void setAlarm(uint8_t hund, uint8_t sec, uint8_t min, uint8_t hour, uint8_t dayOfMonth, uint8_t month); // Set alarm time
    void setAlarmMode(uint8_t mode); //Set the RTC alarm repeat interval
    void attachInterrupt(); //Attach the RTC alarm interrupt
    void detachInterrupt(); //Detach the RTC alarm interrupt

    uint32_t weekday; //0 to 6 representing the day of the week
    uint32_t century;
    uint32_t year;
    uint32_t month;
    uint32_t dayOfMonth;    
    uint32_t hour;
    uint32_t minute;
    uint32_t seconds;
    uint32_t hundredths;
    const char *textWeekday;

    uint32_t alarmWeekday; //0 to 6 representing the day of the week
    uint32_t alarmMonth;
    uint32_t alarmDayOfMonth;    
    uint32_t alarmHour;
    uint32_t alarmMinute;
    uint32_t alarmSeconds;
    uint32_t alarmHundredths;
    const char *alarmTextWeekday;
};

extern Apollo3RTC RTC;

#endif // _APOLLO3_LIBRARIES_RTC_H_
