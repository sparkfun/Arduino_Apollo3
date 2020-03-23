/*
  LowPower_WithWorkAndGPIOInterrupt 
  Adapted by Stephen Fordyce 2020-03-23 from:
  Artemis Low Power: How low can we go?
  By: Nathan Seidle
  SparkFun Electronics
  Date: February 26th, 2020
  License: This code is public domain.
*/

uint32_t msToSleep = 5000; //This is the user editable number of ms to sleep between RTC checks
#define TIMER_FREQ 32768L //Counter/Timer 6 will use the 32kHz clock
uint32_t sysTicksToSleep = msToSleep * TIMER_FREQ / 1000;

const byte STATUS_LED = 14;//13 for Redboard onboard LED, 19 for Nano onboard LED
const byte INPUT_BUTTON = 19;//You'll have to add one, it needs to connect to GND for active

bool awakeFlag = true;  //Stops wakeFromSleep() being run if the system is already awake


// GPIO Interrupt Service Routine
void myGPIO_ISR(void)
{
  detachInterrupt(digitalPinToInterrupt(INPUT_BUTTON));  //Stop interrupt from being triggered again
  wakeFromSleep();  //Without waking the processor properly, nothing more will happen.  This means wakeFromSleep() is called twice though. 
//  am_hal_stimer_compare_delta_set(6, 0);  //Or, force the timer to run out, and resume from where you left off.
}


void setup(void) {
  Serial.begin(115200);
  Serial.println("Artemis Low Power (with timer & GPIO wakeup) Example");
  pinMode(STATUS_LED, OUTPUT);
  pinMode(INPUT_BUTTON, INPUT_PULLUP);
  //Initialise stuff like I2C/Wire/SPI here
}


void loop(void) {
  Serial.println("Starting loop, and flashing LED");
  digitalWrite(STATUS_LED, HIGH);
  delay(200);
  digitalWrite(STATUS_LED, LOW);
  Serial.println("Phew, that was hard work, sleep for 4 seconds");
  msToSleep = 4000;
  sysTicksToSleep = msToSleep * TIMER_FREQ / 1000;
  goToSleep();
}


//Power everything down and wait for interrupt wakeup
void goToSleep()
{
  attachInterrupt(digitalPinToInterrupt(INPUT_BUTTON), myGPIO_ISR, FALLING);
  
  //End stuff like I2C/Wire/SPI here

  power_adc_disable(); //Power down ADC. It it started by default before setup().

  Serial.println("Going to sleep");
  delay(50);  //Wait for serial to finish
  Serial.end(); //Power down UART(s)
  awakeFlag = false;

  //Disable all pads except the interrupt button
  for (int x = 0 ; x < 50 ; x++)
  {
    if(x != INPUT_BUTTON)
      am_hal_gpio_pinconfig(x , g_AM_HAL_GPIO_DISABLE);
  }

  //We use counter/timer 6 to cause us to wake up from sleep but 0 to 7 are available
  //CT 7 is used for Software Serial. All CTs are used for Servo.
  am_hal_stimer_int_clear(AM_HAL_STIMER_INT_COMPAREG); //Clear CT6
  am_hal_stimer_int_enable(AM_HAL_STIMER_INT_COMPAREG); //Enable C/T G=6

  //Use the lower power 32kHz clock. Use it to run CT6 as well.
  am_hal_stimer_config(AM_HAL_STIMER_CFG_CLEAR | AM_HAL_STIMER_CFG_FREEZE);
  am_hal_stimer_config(AM_HAL_STIMER_XTAL_32KHZ | AM_HAL_STIMER_CFG_COMPARE_G_ENABLE);

  //Setup interrupt to trigger when the number of ms have elapsed
  am_hal_stimer_compare_delta_set(6, sysTicksToSleep);

  //Power down Flash, SRAM, cache
  am_hal_pwrctrl_memory_deepsleep_powerdown(AM_HAL_PWRCTRL_MEM_CACHE); //Turn off CACHE
  am_hal_pwrctrl_memory_deepsleep_powerdown(AM_HAL_PWRCTRL_MEM_FLASH_512K); //Turn off everything but lower 512k
  am_hal_pwrctrl_memory_deepsleep_powerdown(AM_HAL_PWRCTRL_MEM_SRAM_64K_DTCM); //Turn off everything but lower 64k
  //am_hal_pwrctrl_memory_deepsleep_powerdown(AM_HAL_PWRCTRL_MEM_ALL); //Turn off all memory (doesn't recover)

  // Enable interrupts to the core.  
  am_hal_interrupt_master_enable(); 

  //Enable the timer interrupt in the NVIC.
  NVIC_EnableIRQ(STIMER_CMPR6_IRQn);

  //Go to Deep Sleep.
  am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);

  /////////////////////////////////////////////////////////////////////
  //<Pause here while sleeping> (and/or while interrupt routines run)//
  /////////////////////////////////////////////////////////////////////

  //Turn off timer interrupt
  NVIC_DisableIRQ(STIMER_CMPR6_IRQn);

  //Turn off GPIO interrupt
  detachInterrupt(digitalPinToInterrupt(INPUT_BUTTON));
  
  //We're BACK!
  wakeFromSleep();
  Serial.println("End of goToSleep()");
  Serial.println();
}


//Power everything up gracefully
void wakeFromSleep()
{
  if(awakeFlag) //Already awake
    return;
  
  //Power up SRAM, turn on entire Flash
  am_hal_pwrctrl_memory_deepsleep_powerdown(AM_HAL_PWRCTRL_MEM_MAX);
  
  //Go back to using the main clock
  am_hal_stimer_int_enable(AM_HAL_STIMER_INT_OVERFLOW);
  NVIC_EnableIRQ(STIMER_IRQn);
  am_hal_stimer_config(AM_HAL_STIMER_CFG_CLEAR | AM_HAL_STIMER_CFG_FREEZE);
  am_hal_stimer_config(AM_HAL_STIMER_HFRC_3MHZ);

  //Turn on ADC
  ap3_adc_setup();

  //Set any pinModes
  pinMode(STATUS_LED, OUTPUT);
  pinMode(INPUT_BUTTON, INPUT_PULLUP);

  //Optional - start again (will never reach the end of goToSleep() or wakeFromSleep() though)
  //Note - global variables will be preserved if you don't power down SRAM (comment out the line)
//setup();
  
  //Restart Serial
  Serial.begin(115200);
  delay(10);
  Serial.println("Back on");
  awakeFlag = true;

  //Initialise stuff like I2C/Wire/SPI here 
}


//Called once number of milliseconds has passed
extern "C" void am_stimer_cmpr6_isr(void)
{
  uint32_t ui32Status = am_hal_stimer_int_status_get(false);
  if (ui32Status & AM_HAL_STIMER_INT_COMPAREG)
  {
    am_hal_stimer_int_clear(AM_HAL_STIMER_INT_COMPAREG);
  }
}
