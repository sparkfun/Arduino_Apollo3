#ifndef WDT_H
#define WDT_H

#include <Arduino.h>

class APM3_WDT
{
public:
  APM3_WDT();

  // Watchdog timer configuration structure
  am_hal_wdt_config_t watchdogConfig = {

    // Configuration values for generated watchdog timer event.
    .ui32Config = AM_HAL_WDT_LFRC_CLK_16HZ | AM_HAL_WDT_ENABLE_RESET | AM_HAL_WDT_ENABLE_INTERRUPT,

    // Number of watchdog timer ticks allowed before a watchdog interrupt event is generated
    .ui16InterruptCount = 16 * 5, // Set WDT interrupt timeout for 5 seconds (16 * 5) / 16 = 5

    // Number of watchdog timer ticks allowed before the watchdog will issue a system reset
    .ui16ResetCount = 16 * 10 // Set WDT reset timeout for 15 seconds (16 * 10 ) / 16 = 10
  };

  void configure(); // Configure watchdog
  void start(); // Starts the watchdog timer
  void stop(); // Disables the watchdog timer
  void restart(); // Restarts the watchdog timer ("Pets" the dog)
  void clear(); // Clears the watchdog interrupt

};
#endif //WDT_H
