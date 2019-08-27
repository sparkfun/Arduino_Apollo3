#include "BLE_example.h"

void setup() {

  #ifdef DEBUG
    SERIAL_PORT.begin(115200);
    delay(1000);
    SERIAL_PORT.printf("Apollo3 Arduino BLE Example. Compiled: %s\n", __TIME__);
  #endif

  pinMode(LED_BUILTIN, OUTPUT);
  set_led_low();

  //
  // Boot the radio.
  //
  HciDrvRadioBoot(0);

  //
  // Initialize the main ExactLE stack.
  //
  exactle_stack_init();

  //
  // Start the "Nus" profile.
  //
  NusStart();
}

void loop() {
      
      //
      // Calculate the elapsed time from our free-running timer, and update
      // the software timers in the WSF scheduler.
      //
      update_scheduler_timers();
      wsfOsDispatcher();

      //
      // Enable an interrupt to wake us up next time we have a scheduled event.
      //
      set_next_wakeup();

      am_hal_interrupt_master_disable();

      //
      // Check to see if the WSF routines are ready to go to sleep.
      //
      if ( wsfOsReadyToSleep() )
      {
          am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
      }
      am_hal_interrupt_master_enable();
}