/*
  
*/

#include "WDT.h"

// Constructor
APM3_WDT::APM3_WDT()
{
    // The watchdog only runs off of the LFRC
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_LFRC_START, 0);
}


// Configure the watchdog
void APM3_WDT::configure()
{
  am_hal_wdt_init(&watchdogConfig);
}

// Enable the watchdog
void APM3_WDT::start()
{
  // Enable the interrupt for the watchdog in the NVIC
  NVIC_EnableIRQ(WDT_IRQn);
  am_hal_wdt_start();
}

// Disable the watchdog
void APM3_WDT::stop()
{
  // Disable the interrupt for the watchdog in the NVIC
  NVIC_DisableIRQ(WDT_IRQn);
  // Disable the watchdog timer tick by clearing the 'enable' bit in the
  // watchdog configuration register
  am_hal_wdt_halt();
}

// Restart the watchdog
void APM3_WDT::restart()
{
  am_hal_wdt_restart();
}

// Clear the watchdog interrupt
void APM3_WDT::clear()
{
  am_hal_wdt_int_clear();
}