#include "Arduino.h"
#include "ap3_initialization.h"

// Weak empty variant initialization function.
// May be redefined by variant files.
void ap3_variant_init( void ) __attribute__((weak));
void ap3_variant_init( void ) { }

// Initialize C library
extern "C" void __libc_init_array(void);
extern "C" void _init( void ){
  // Empty definition to resolve linker error within '__libc_init_array'
}
extern "C" void end( void ){
  // Empty definition to satisfy linker error in '_sbrk'
}

extern "C" int main( void )
{
  ap3_init();
  __libc_init_array();

  ap3_variant_init();
  setup();
  for (;;)
  {
    loop();
  }
  return 0;
}






// // A potential future version of this core could be implemented using FreeRTOS by default. 
// #include "FreeRTOS.h"
// #include "task.h"
// #include "queue.h"
// #include "portmacro.h"
// #include "portable.h"

// TaskHandle_t xLoopTask;

// void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName) __attribute__ ((weak));

// void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
// {
//     (void) pcTaskName;
//     (void) pxTask;
//     //
//     // Run time stack overflow checking is performed if
//     // configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
//     // function is called if a stack overflow is detected.
//     //
//     while (1)
//     {
//         __asm("BKPT #0\n") ; // Break into the debugger
//     }
// }


// void loopTask( void* pvParameters )
// {
//   // setup();
//   for(;;){
//     // loop();
//     am_bsp_uart_string_print("I'm a task yo\r\n");
//     vTaskDelay(1000/portTICK_PERIOD_MS);
//   }
// }

// extern "C" int main( void )
// {
//   setup();
//   am_bsp_uart_string_print("I'm about to start FreeRTOS!\r\n");
//   xTaskCreate(loopTask, "Loop", AP3_ARD_FREERTOS_LOOP_STACK_DEPTH, NULL, AP3_ARD_FREERTOS_LOOP_PRIORITY, &xLoopTask);
//   vTaskStartScheduler();
// }