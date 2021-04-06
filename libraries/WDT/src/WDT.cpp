/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "WDT.h"

Apollo3WDT wdt;

// Constructor
Apollo3WDT::Apollo3WDT()
{
    // The watchdog only runs off of the LFRC
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_LFRC_START, 0);
    am_hal_wdt_init(&watchdogConfig);
}

// Configure the watchdog timer
// clock:     set the LFRC clock frequency of the watchdog timer (see setClock below)
// interrupt: number of watchdog timer ticks allowed before a watchdog interrupt event is generated
// reset:     number of watchdog timer ticks allowed before the watchdog will issue a system reset
void Apollo3WDT::configure(uint8_t clock, uint8_t interrupt, uint8_t reset)
{
  if(clock <= 4)
  {
    WDT->CFG_b.CLKSEL = clock;
  }
  else
  {
    WDT->CFG_b.CLKSEL = WDT_CFG_CLKSEL_16HZ; // Default to 16Hz LFRC clock divider
  }
  WDT->CFG_b.INTVAL = interrupt; 
  WDT->CFG_b.RESVAL = reset; 
}

// Set the LFRC clock frequency of the watchdog timer
// WDT_OFF     WDT_CFG_CLKSEL_OFF      // = 0    Low Power Mode. This setting disables the watch dog timer
// WDT_128HZ   WDT_CFG_CLKSEL_128HZ    // = 1,   128 Hz LFRC clock
// WDT_16HZ    WDT_CFG_CLKSEL_16HZ     // = 2    16 Hz LFRC clock
// WDT_1HZ     WDT_CFG_CLKSEL_1HZ      // = 3,   1 Hz LFRC clock
// WDT_1_16HZ  WDT_CFG_CLKSEL_1_16HZ   // = 4,   1/16th Hz LFRC clock
void Apollo3WDT::setClock(uint8_t clock)
{
  if(clock <= 4)
  {
    WDT->CFG_b.CLKSEL = clock;
  }
  else
  {
    WDT->CFG_b.CLKSEL = WDT_CFG_CLKSEL_16HZ; // Default to 16Hz LFRC clock divider
  }
}

// Set number of watchdog timer ticks allowed before a watchdog interrupt event is generated
void Apollo3WDT::setInterrupt(uint8_t interrupt)
{
  WDT->CFG_b.INTVAL = interrupt;   
}

// Set number of watchdog timer ticks allowed before the watchdog will issue a system reset
void Apollo3WDT::setReset(uint8_t reset)
{
  WDT->CFG_b.RESVAL = reset; 
}

// Enable the watchdog
void Apollo3WDT::start()
{
  am_hal_interrupt_master_enable(); // Enable interrupts to the core
  NVIC_EnableIRQ(WDT_IRQn); // Enable the interrupt for the watchdog in the NVIC
  am_hal_wdt_start();
}

// Disable the watchdog
void Apollo3WDT::stop()
{
  // Disable the interrupt for the watchdog in the NVIC
  NVIC_DisableIRQ(WDT_IRQn);
  // Disable the watchdog timer tick by clearing the 'enable' bit in the
  // watchdog configuration register
  am_hal_wdt_halt();
}

// Restart the watchdog
void Apollo3WDT::restart()
{
  am_hal_wdt_restart();
}

// Clear the watchdog interrupt
void Apollo3WDT::clear()
{
  am_hal_wdt_int_clear();
}

// Read the current value of watch dog timer counter register
uint32_t Apollo3WDT::getCounter()
{
  return am_hal_wdt_counter_get();
}