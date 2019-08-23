#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include "wsf_types.h"
#include "wsf_trace.h"
#include "wsf_buf.h"

#include "hci_handler.h"
#include "dm_handler.h"
#include "l2c_handler.h"
#include "att_handler.h"
#include "smp_handler.h"
#include "l2c_api.h"
#include "att_api.h"
#include "smp_api.h"
#include "app_api.h"
#include "hci_core.h"
#include "hci_drv.h"
#include "hci_drv_apollo.h"
#include "hci_drv_apollo3.h"

#include "am_mcu_apollo.h"
#include "am_util.h"

#include "nus_api.h"
#include "app_ui.h"

#include "wsf_msg.h"

#ifdef __cplusplus
}
#endif

//*****************************************************************************
//
// Forward declarations.
//
//*****************************************************************************
void exactle_stack_init(void);
void scheduler_timer_init(void);
void update_scheduler_timers(void);
void set_next_wakeup(void);
void button_handler(wsfEventMask_t event, wsfMsgHdr_t *pMsg);
extern void AppUiBtnTest(uint8_t btn);


//*****************************************************************************
//
// Timer for buttons.
//
//*****************************************************************************
wsfHandlerId_t ButtonHandlerId;
wsfTimer_t ButtonTimer;

//*****************************************************************************
//
// Timer configuration macros.
//
//*****************************************************************************
#define MS_PER_TIMER_TICK           10  // Milliseconds per WSF tick
#define CLK_TICKS_PER_WSF_TICKS     5   // Number of CTIMER counts per WSF tick.

//*****************************************************************************
//
// WSF buffer pools.
//
//*****************************************************************************
#define WSF_BUF_POOLS               4

// Important note: the size of g_pui32BufMem should includes both overhead of internal
// buffer management structure, wsfBufPool_t (up to 16 bytes for each pool), and pool
// description (e.g. g_psPoolDescriptors below).

// Memory for the buffer pool
static uint32_t g_pui32BufMem[(WSF_BUF_POOLS*16
         + 16*8 + 32*4 + 64*6 + 280*8) / sizeof(uint32_t)];

// Default pool descriptor.
static wsfBufPoolDesc_t g_psPoolDescriptors[WSF_BUF_POOLS] =
{
    {  16,  8 },
    {  32,  4 },
    {  64,  6 },
    { 280,  8 }
};

////*****************************************************************************
////
//// Enable printing to the console.
////
////*****************************************************************************
//void
//enable_print_interface(void)
//{
//    // //
//    // // Initialize a debug printing interface.
//    // //
//    // am_bsp_itm_printf_enable();
//    am_bsp_uart_printf_enable();
//}

//*****************************************************************************
//
// Tracking variable for the scheduler timer.
//
//*****************************************************************************
uint32_t g_ui32LastTime = 0;
extern "C" void radio_timer_handler(void);

//*****************************************************************************
//
// Initialization for the ExactLE stack.
//
//*****************************************************************************
void exactle_stack_init(void){
    wsfHandlerId_t handlerId;

    //
    // Set up timers for the WSF scheduler.
    //
    scheduler_timer_init();
    WsfTimerInit();

    //
    // Initialize a buffer pool for WSF dynamic memory needs.
    //
    WsfBufInit(sizeof(g_pui32BufMem), (uint8_t*)g_pui32BufMem, WSF_BUF_POOLS, g_psPoolDescriptors);

    //
    // Initialize security.
    //
    SecInit();
    SecAesInit();
    SecCmacInit();
    SecEccInit();

    //
    // Set up callback functions for the various layers of the ExactLE stack.
    //
    handlerId = WsfOsSetNextHandler(HciHandler);
    HciHandlerInit(handlerId);

    handlerId = WsfOsSetNextHandler(DmHandler);
    DmDevVsInit(0);
    DmAdvInit();
    DmConnInit();
    DmConnSlaveInit();
    DmSecInit();
    DmSecLescInit();
    DmPrivInit();
    DmHandlerInit(handlerId);

    handlerId = WsfOsSetNextHandler(L2cSlaveHandler);
    L2cSlaveHandlerInit(handlerId);
    L2cInit();
    L2cSlaveInit();

    handlerId = WsfOsSetNextHandler(AttHandler);
    AttHandlerInit(handlerId);
    AttsInit();
    AttsIndInit();
    AttcInit();

    handlerId = WsfOsSetNextHandler(SmpHandler);
    SmpHandlerInit(handlerId);
    SmprInit();
    SmprScInit();
    HciSetMaxRxAclLen(251);

    handlerId = WsfOsSetNextHandler(AppHandler);
    AppHandlerInit(handlerId);

    handlerId = WsfOsSetNextHandler(NusHandler);
    NusHandlerInit(handlerId);

//    ButtonHandlerId = WsfOsSetNextHandler(button_handler);

    handlerId = WsfOsSetNextHandler(HciDrvHandler);
    HciDrvHandlerInit(handlerId);
}



//*****************************************************************************
//
// Set up a pair of timers to handle the WSF scheduler.
//
//*****************************************************************************
void
scheduler_timer_init(void)
{
    //
    // One of the timers will run in one-shot mode and provide interrupts for
    // scheduled events.
    //
    am_hal_ctimer_clear(0, AM_HAL_CTIMER_TIMERA);
    am_hal_ctimer_config_single(0, AM_HAL_CTIMER_TIMERA,
                                (AM_HAL_CTIMER_INT_ENABLE |
                                 AM_HAL_CTIMER_LFRC_512HZ |
                                 AM_HAL_CTIMER_FN_ONCE));

    //
    // The other timer will run continuously and provide a constant time-base.
    //
    am_hal_ctimer_clear(0, AM_HAL_CTIMER_TIMERB);
    am_hal_ctimer_config_single(0, AM_HAL_CTIMER_TIMERB,
                                 (AM_HAL_CTIMER_LFRC_512HZ |
                                 AM_HAL_CTIMER_FN_CONTINUOUS));

    //
    // Start the continuous timer.
    //
    am_hal_ctimer_start(0, AM_HAL_CTIMER_TIMERB);

    //
    // Enable the timer interrupt.
    //
    am_hal_ctimer_int_register(AM_HAL_CTIMER_INT_TIMERA0, radio_timer_handler);
    am_hal_ctimer_int_enable(AM_HAL_CTIMER_INT_TIMERA0);
    NVIC_EnableIRQ(CTIMER_IRQn);
}

//*****************************************************************************
//
// Calculate the elapsed time, and update the WSF software timers.
//
//*****************************************************************************
void
update_scheduler_timers(void)
{
    uint32_t ui32CurrentTime, ui32ElapsedTime;

    //
    // Read the continuous timer.
    //
    ui32CurrentTime = am_hal_ctimer_read(0, AM_HAL_CTIMER_TIMERB);

    //
    // Figure out how long it has been since the last time we've read the
    // continuous timer. We should be reading often enough that we'll never
    // have more than one overflow.
    //
    ui32ElapsedTime = (ui32CurrentTime >= g_ui32LastTime ?
                       (ui32CurrentTime - g_ui32LastTime) :
                       (0x10000 + ui32CurrentTime - g_ui32LastTime));

    //
    // Check to see if any WSF ticks need to happen.
    //
    if ( (ui32ElapsedTime / CLK_TICKS_PER_WSF_TICKS) > 0 )
    {
        //
        // Update the WSF timers and save the current time as our "last
        // update".
        //
        WsfTimerUpdate(ui32ElapsedTime / CLK_TICKS_PER_WSF_TICKS);

        g_ui32LastTime = ui32CurrentTime;
    }
}

//*****************************************************************************
//
// Set a timer interrupt for the next upcoming scheduler event.
//
//*****************************************************************************
void
set_next_wakeup(void)
{
    bool_t bTimerRunning;
    wsfTimerTicks_t xNextExpiration;

    //
    // Stop and clear the scheduling timer.
    //
    am_hal_ctimer_stop(0, AM_HAL_CTIMER_TIMERA);
    am_hal_ctimer_clear(0, AM_HAL_CTIMER_TIMERA);

    //
    // Check to see when the next timer expiration should happen.
    //
    xNextExpiration = WsfTimerNextExpiration(&bTimerRunning);

    //
    // If there's a pending WSF timer event, set an interrupt to wake us up in
    // time to service it. Otherwise, set an interrupt to wake us up in time to
    // prevent a double-overflow of our continuous timer.
    //
    if ( xNextExpiration )
    {
        am_hal_ctimer_period_set(0, AM_HAL_CTIMER_TIMERA,
                                 xNextExpiration * CLK_TICKS_PER_WSF_TICKS, 0);
    }
    else
    {
        am_hal_ctimer_period_set(0, AM_HAL_CTIMER_TIMERA, 0x8000, 0);
    }

    //
    // Start the scheduling timer.
    //
    am_hal_ctimer_start(0, AM_HAL_CTIMER_TIMERA);
}

//*****************************************************************************
//
// Interrupt handler for the CTIMERs
//
//*****************************************************************************
extern "C" void radio_timer_handler(void){
    // Signal radio task to run

    WsfTaskSetReady(0, 0);
}

////*****************************************************************************
////
//// Poll the buttons.
////
////*****************************************************************************
//void
//button_handler(wsfEventMask_t event, wsfMsgHdr_t *pMsg)
//{
//    //
//    // Restart the button timer.
//    //
//    WsfTimerStartMs(&ButtonTimer, 10);
//
//    //
//    // Every time we get a button timer tick, check all of our buttons.
//    //
//    am_devices_button_array_tick(am_bsp_psButtons, AM_BSP_NUM_BUTTONS);
//
//    //
//    // If we got a a press, do something with it.
//    //
//    if ( am_devices_button_released(am_bsp_psButtons[0]) )
//    {
//        am_util_debug_printf("Got Button 0 Press\n");
//        AppUiBtnTest(APP_UI_BTN_1_SHORT);
//    }
//
//    if ( am_devices_button_released(am_bsp_psButtons[1]) )
//    {
//        am_util_debug_printf("Got Button 1 Press\n");
//        AppUiBtnTest(APP_UI_BTN_1_SHORT);
//    }
//}
//
////*****************************************************************************
////
//// Sets up a button interface.
////
////*****************************************************************************
//void
//setup_buttons(void)
//{
//    //
//    // Enable the buttons for user interaction.
//    //
//    am_devices_button_array_init(am_bsp_psButtons, AM_BSP_NUM_BUTTONS);
//
//    //
//    // Start a timer.
//    //
//    ButtonTimer.handlerId = ButtonHandlerId;
//    WsfTimerStartSec(&ButtonTimer, 2);
//}

//*****************************************************************************
//
// Interrupt handler for the CTIMERs
//
//*****************************************************************************
extern "C" void am_ctimer_isr(void){
    uint32_t ui32Status;

    //
    // Check and clear any active CTIMER interrupts.
    //
    ui32Status = am_hal_ctimer_int_status_get(true);
    am_hal_ctimer_int_clear(ui32Status);

    //
    // Run handlers for the various possible timer events.
    //
    am_hal_ctimer_int_service(ui32Status);
}

//*****************************************************************************
//
// Interrupt handler for BLE
//
//*****************************************************************************
extern "C" void am_ble_isr(void){
    HciDrvIntService();
}


void setup() {
  // put your setup code here, to run once:

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

  while (TRUE)
  {
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
}

void loop() {
  // put your main code here, to run repeatedly:

}
