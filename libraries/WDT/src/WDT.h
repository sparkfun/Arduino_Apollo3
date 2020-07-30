#ifndef WDT_H
#define WDT_H

#include <Arduino.h>

class APM3_WDT
{
public:
  APM3_WDT();

  void configure(uint32_t clock, uint16_t interrupt, uint16_t reset); // Configure watchdog
  void enable(); // Enable the watchdog
  void disable(); // Disable the watchdog
  void restart(); // Restart the watchdog
  void clear() // Clear the watchdog interrupt

};
#endif //WDT_H
