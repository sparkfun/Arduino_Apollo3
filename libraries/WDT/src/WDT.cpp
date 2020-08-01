#include "WDT.h"

// Constructor
APM3_WDT::APM3_WDT()
{
    // The watchdog only runs off of the LFRC
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_LFRC_START, 0);
    am_hal_wdt_init(&watchdogConfig);
}

// Configure the watchdog timer
void APM3_WDT::configure(uint8_t clock, uint8_t interrupt, uint8_t reset)
{
  WDT->CFG_b.CLKSEL = clock; 
  WDT->CFG_b.INTVAL = interrupt; 
  WDT->CFG_b.RESVAL = reset; 
}

// Set the LFRC clock frequency of the watchdog timer
void APM3_WDT::setClock(uint8_t clock)
{
  WDT->CFG_b.CLKSEL = clock; 
}

// Set number of watchdog timer ticks allowed before a watchdog interrupt event is generated
void APM3_WDT::setInterrupt(uint8_t interrupt)
{
  WDT->CFG_b.INTVAL = interrupt;   
}

// Set number of watchdog timer ticks allowed before the watchdog will issue a system reset
void APM3_WDT::setReset(uint8_t reset)
{
  WDT->CFG_b.RESVAL = reset; 
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

// Read the current value of watch dog timer counter register
uint32_t APM3_WDT::getCounter()
{
  return am_hal_wdt_counter_get();
}

