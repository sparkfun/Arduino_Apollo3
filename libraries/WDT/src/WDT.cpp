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
void APM3_WDT::configure(clock, interrupt, reset)
{

}

// Enable the watchdog
void APM3_WDT::enable()
{
  // Enable the interrupt for the watchdog in the NVIC
  NVIC_EnableIRQ(WDT_IRQn);
  am_hal_wdt_start();
}

// Disable the watchdog
void APM3_WDT::disable()
{
  // Enable the interrupt for the watchdog in the NVIC
  NVIC_DisableIRQ(WDT_IRQn);
  am_hal_wdt_int_disable();
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