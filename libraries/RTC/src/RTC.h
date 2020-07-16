#ifndef RTC_H
#define RTC_H

#include <Arduino.h>

class APM3_RTC
{
public:
  APM3_RTC();

  void getTime(); //Query the RTC for the current time/date
  uint32_t getEpoch(); //Return the current RTC time/date as UNIX Epoch time 
  
  void setTime(uint8_t hour, uint8_t min, uint8_t sec, uint8_t hund,
               uint8_t dayOfMonth, uint8_t month, uint16_t year); //Set current time to provided hundredths/seconds/etc
  void setToCompilerTime(); //Set to time when sketch was compiled
  void setEpoch(uint32_t ts); //Set current time to provided UNIX Epoch time

  void getAlarm(); //Query the RTC for the current alarm time/date
  void setAlarm(uint8_t hour, uint8_t min, uint8_t sec, uint8_t hund, uint8_t dayOfMonth, uint8_t month); //Set alarm time to provided hundredths/seconds/etc
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

private:
  //Helper functions to convert compiler date/time to ints
  int toVal(char const *pcAsciiStr);
  int mthToIndex(char const *pcMon);
};
#endif //RTC_H
