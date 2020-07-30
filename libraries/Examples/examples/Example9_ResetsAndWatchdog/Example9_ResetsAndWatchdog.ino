/* Author: Stephen Fordyce (adapted from WDT example code by Adam Garbo - https://forum.sparkfun.com/viewtopic.php?f=169&t=52431&p=213296&hilit=watchdog#p213296)
 Created: 24th July 2020
 License: MIT. See SparkFun Arduino Apollo3 Project for more information

 This example demonstrates use of the watchdog timer, also different ways of resetting the Artemis as well as how to determine the last reset type
*/

// Global variables
volatile uint8_t watchdogCounter = 0; // Watchdog interrupt counter
uint32_t resetStatus = 0; // Reset status register
bool testWatchdogResetFlag = false;
bool watchdogInterruptCallsEmergencyReset = false;
bool resetWDTEveryLoop = true;

// Watchdog timer configuration structure.
am_hal_wdt_config_t g_sWatchdogConfig = {
  //Substitude other values for AM_HAL_WDT_LFRC_CLK_16HZ to increase/decrease the range
  .ui32Config = AM_HAL_WDT_LFRC_CLK_16HZ | AM_HAL_WDT_ENABLE_RESET | AM_HAL_WDT_ENABLE_INTERRUPT, // Configuration values for generated watchdog timer event.
  //****** EVEN THOUGH THESE IMPLY 16-BIT, THEY ARE ONLY 8-BIT - 255 MAX!
  .ui16InterruptCount = 120, //MAX 255! // Set WDT interrupt timeout for 5 seconds (120 / 16 = 8).  // Number of watchdog timer ticks allowed before a watchdog interrupt event is generated.
  .ui16ResetCount = 160 //MAX 255! // Set WDT reset timeout for 15 seconds (160 / 16 = 10).  // Number of watchdog timer ticks allowed before the watchdog will issue a system reset.
};

void setup(void) 
{
  pinMode(LED_BUILTIN, OUTPUT);  
  Serial.begin(115200); delay(10);
  Serial.println();Serial.println();
  Serial.println("****Artemis Reset & Watchdog Reset Example****");
  
  printResetReason(); //Explain reason for reset  
  am_hal_reset_control(AM_HAL_RESET_CONTROL_STATUSCLEAR, 0);  // Clear reset status register for next time we reset.
  delay(1000);

  startWatchdogTimer();  //Initialise and start the watchdog timer, in case the program gets stuck in a while loop or waiting for user input
  printWatchdogDetails();
  delay(2000);
  
  Serial.println("\nRunning a time-consuming loop with watchdog running");
  for(int i=0; i<5; i++)
  {
    delay(1000); //Phew, a bunch of hard processing
    printWatchdogValue();
  }
  Serial.println("Yikes, the time kept increasing, even though we were doing legitimate work\n");

  Serial.println("\nRunning a time-consuming loop that restarts the watchdog timer each loop");
  for(int i=0; i<5; i++)
  {
    delay(1000); //Phew, a bunch of hard processing
    printWatchdogValue();
    am_hal_wdt_restart(); // "Pet" the dog. // Restart the watchdog.
  }
  Serial.println("Plenty of time, but the watchdog timer will still catch a freeze\n");
  delay(2000);
}

void loop(void) 
{
  Serial.println("Start of loop()");
  if(resetWDTEveryLoop)
    am_hal_wdt_restart(); // "Pet" the dog. // Restart the watchdog. (every loop run seems good)

  if(resetWDTEveryLoop)
  {
    Serial.println("Enter 'r' to do a scheduled software reset");
    Serial.println("Enter 's' to stop resetting the watchdog timer each loop");
    delay(1000);
    char option = Serial.read();
    if(option == 'r')
      myScheduledReset();
    if(option == 's')
    {
      Serial.println("I take no responsibility for the consequences!"); 
      Serial.println("(the watchdog interrupt routine when triggered will reset the timer 3 times, watch the timer values to see when it's reset by the ISR)"); 
      delay(2000);
      resetWDTEveryLoop = false;
    }
  }

  if(resetWDTEveryLoop == false)
  {
    Serial.println("Enter 'e' to let the watchdog interrupt call emergencyReset()");
    delay(1000);
    char option = Serial.read();
    if(option == 'e')
    {
      Serial.println("Cool, that'll happen in a sec"); delay(20);
      watchdogInterruptCallsEmergencyReset = true;
    }
  }
  
  if(testWatchdogResetFlag)
  {
    Serial.println("Just about to go down the rabbit hole...");
    delay(20);  //Let serial buffer clear
    while(1);   //Intentionally get stuck, and hope that the watchdog timer saves the day
  }

  printWatchdogValue();
  Serial.println();
  delay(10);

  //Optional code for sleeping - just halt the WDT before you disable everything, then re-initialise the WDT after waking up.  Tested elsewhere, and does not have any appreciable impact on sleep currents or performance
  //am_hal_wdt_halt();
  //sleepForABit();
  //startWatchdog();  
}

// Interrupt handler for the watchdog.
extern "C" void am_watchdog_isr(void) 
{
  am_hal_wdt_int_clear(); // Clear the watchdog interrupt.
  if(watchdogInterruptCallsEmergencyReset)
    emergencyReset();

  if ( watchdogCounter < 3 )  // Catch the first three watchdog interrupts, but let the fourth through untouched.
  {
    digitalWrite(LED_BUILTIN, LOW);
    am_hal_wdt_restart(); // "Pet" the dog (reset the timer)
  }
  else 
  {
    digitalWrite(LED_BUILTIN, HIGH); // Indicator that a reset will occur.
    testWatchdogResetFlag = true;
  }

  watchdogCounter++;  // Increment the number of watchdog interrupts.
}

void startWatchdogTimer()
{ 
  am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_LFRC_START, 0); // LFRC must be turned on for this example as the watchdog only runs off of the LFRC.
  am_hal_wdt_init(&g_sWatchdogConfig);  // Configure the watchdog.
  NVIC_EnableIRQ(WDT_IRQn); // Enable the interrupt for the watchdog in the NVIC.
  am_hal_interrupt_master_enable();
  am_hal_wdt_start(); // Enable the watchdog.
}

//IMPORTANT: IF THIS IS CALLED BY THE WATCHDOG TIMER INTERRUPT ROUTINE, IT  NEEDS TO BE QUICK (NO PRINTING TO SERIAL OR DELAYS)
void emergencyReset()
{
  //Optional: write some bits to flash/EEPROM to help with recovery  

  am_hal_reset_control(AM_HAL_RESET_CONTROL_SWPOI,0); //Reset with option "Software Power On Initialization" SWPOI
//  am_hal_reset_control(AM_HAL_RESET_CONTROL_SWPOR,0); //Reset with option SWPOR (same as SWPOI but different am_hal_reset_status bit)  
}

void myScheduledReset()
{
  //Optional: write some bits to flash/EEPROM to help with recovery  

//  am_hal_reset_control(AM_HAL_RESET_CONTROL_SWPOI,0); //Reset with option "Software Power On Initialization" SWPOI
  am_hal_reset_control(AM_HAL_RESET_CONTROL_SWPOR,0); //Reset with option SWPOR (same as SWPOI but different am_hal_reset_status bit)  
}

void printWatchdogDetails()
{
  Serial.print("Interrupt Count = "); Serial.print(g_sWatchdogConfig.ui16InterruptCount ); Serial.println(" ticks");
  Serial.print("Reset Count = "); Serial.print(g_sWatchdogConfig.ui16ResetCount); Serial.println(" ticks");
  
  // Print out reset status register.  
  am_hal_reset_status_t sStatus; // (Note: See am_hal_reset.h for RESET status structure)
  am_hal_reset_status_get(&sStatus);
  resetStatus = sStatus.eStatus;
  char rStatus[30];
  sprintf(rStatus, "Reset Status Register = 0x%x", resetStatus);  // (Note: Watch Dog Timer reset = 0x40)
  Serial.println(rStatus);
}

void printWatchdogValue()
{
  Serial.print("Watchdog timer:");
  Serial.println(am_hal_wdt_counter_get());
}

void printResetReason()
{
   am_hal_reset_status_t resetResult;
  uint32_t resultOk = am_hal_reset_status_get(&resetResult);
  Serial.print("Reset reason: ");
  if(resultOk == AM_HAL_STATUS_FAIL) Serial.println("Failed to get reset status");
  if(resetResult.bEXTStat) Serial.println("External reset");
  if(resetResult.bPORStat) Serial.println("Power-On reset");
  if(resetResult.bBODStat) Serial.println("Brown-Out reset");
  if(resetResult.bSWPORStat) Serial.println("SW Power-On reset or AIRCR reset - in this example, indicates reset by myScheduledReset()");
  if(resetResult.bSWPOIStat) Serial.println("SW Power On Initialization reset - in this example, indicates reset by emergencyReset()");
  if(resetResult.bDBGRStat) Serial.println("Debugger reset");
  if(resetResult.bWDTStat) Serial.println("Watch Dog Timer reset - nothing in the Watchdog Timer Interrupt routine got to restarting the Watchdog Timer");
  if(resetResult.bBOUnregStat) Serial.println("Unregulated Supply Brownout event");
  if(resetResult.bBOCOREStat) Serial.println("Core Regulator Brownout event");
  if(resetResult.bBOMEMStat) Serial.println("Memory Regulator Brownout event");
  if(resetResult.bBOBLEStat) Serial.println("BLE/Burst Regulator Brownout event");
//  am_hal_reset_control(AM_HAL_RESET_CONTROL_STATUSCLEAR, 0);  // (do this in setup() Clear reset status register for next time we reset.
  Serial.println();
}
