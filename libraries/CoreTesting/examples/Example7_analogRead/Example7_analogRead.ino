/*
  Analog to digital conversion
  By: Nathan Seidle
  SparkFun Electronics
  Date: May 2nd, 2019
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This example shows how to read an analog voltage from various ADC enabled pins.

  Feel like supporting open source hardware?
  Buy a board from SparkFun!
  SparkFun Edge: https://www.sparkfun.com/products/15170

  Hardware Connections:
  Connect an Edge via programmer
  Upload code
  Use a trimpot or other device to send a 0 to 3V (no 5V!) signal to pin 'A16'
  Max reading on ADC is 2V
*/


/*
  Should we pipe HAL print statements to a debug buffer?
  am_util_stdio_printf() overwrite?
*/

//#define A1  13 //A1 on BB is GP13, SE8

#define LED 5 //Should toggle LED on pin D13

// ADC Device Handle.
static void *g_ADCHandle;

// Define the ADC SE0 pin to be used.
const am_hal_gpio_pincfg_t g_AM_PIN_16_ADCSE0 =
{
  .uFuncSel = AM_HAL_PIN_16_ADCSE0,
};

void setup() {
  Serial.begin(9600);
  Serial.println("SparkFun Arduino Apollo3 Analog Read example");

  pinMode(LED, OUTPUT);

  // Set a pin to act as our ADC input
  //am_hal_gpio_pinconfig(13, g_AM_PIN_13_ADCSE8);
  am_hal_gpio_pinconfig(16, g_AM_PIN_16_ADCSE0);

  adc_config();
  //enable14BitAnalog(); //All analogReads will return 14 bit values instead of 12 bit

  //int myValue = analogRead(A1); //Returns 12-bit ADC value by default.
}

void loop() {
  digitalWrite(LED, LOW);

  int myValue14Bit = analogRead14(16); //Always returns 14-bit ADC value
  Serial.print("analog: ");
  Serial.print(myValue14Bit);
  Serial.println();
  delay(50);

  digitalWrite(LED, HIGH);
}

//Maintain a record of which pins are setup as analog
//When user calls analogRead for the first time, setup pin and mark it

uint32_t analogRead14(uint8_t pin)
{
  uint32_t ui32IntMask;
  am_hal_adc_sample_t Sample;
  uint32_t ui32NumSamples = 1;

  am_hal_adc_sw_trigger(g_ADCHandle);

  if (AM_HAL_STATUS_SUCCESS != am_hal_adc_samples_read(g_ADCHandle,
      true,
      NULL,
      &ui32NumSamples,
      &Sample))
  {
    am_util_stdio_printf("Error - ADC sample read failed.\n");
  }

  return (Sample.ui32Sample);
}

// Configure the ADC.
void adc_config(void)
{
  am_hal_adc_config_t           ADCConfig;
  am_hal_adc_slot_config_t      ADCSlotConfig;

  // Initialize the ADC and get the handle.
  if ( AM_HAL_STATUS_SUCCESS != am_hal_adc_initialize(0, &g_ADCHandle) )
  {
    Serial.println("Error - reservation of the ADC instance failed.\n");
  }

  // Power on the ADC.
  if (AM_HAL_STATUS_SUCCESS != am_hal_adc_power_control(g_ADCHandle,
      AM_HAL_SYSCTRL_WAKE,
      false) )
  {
    Serial.println("Error - ADC power on failed.\n");
  }

  // Set up the ADC configuration parameters. These settings are reasonable
  // for accurate measurements at a low sample rate.
  ADCConfig.eClock             = AM_HAL_ADC_CLKSEL_HFRC;
  ADCConfig.ePolarity          = AM_HAL_ADC_TRIGPOL_RISING;
  ADCConfig.eTrigger           = AM_HAL_ADC_TRIGSEL_SOFTWARE;
  ADCConfig.eReference         = AM_HAL_ADC_REFSEL_INT_2P0;
  ADCConfig.eClockMode         = AM_HAL_ADC_CLKMODE_LOW_POWER;
  ADCConfig.ePowerMode         = AM_HAL_ADC_LPMODE0;
  ADCConfig.eRepeat            = AM_HAL_ADC_SINGLE_SCAN;
  if (AM_HAL_STATUS_SUCCESS != am_hal_adc_configure(g_ADCHandle, &ADCConfig))
  {
    Serial.println("Error - configuring ADC failed.\n");
  }

  // Set up an ADC slot
  ADCSlotConfig.eMeasToAvg      = AM_HAL_ADC_SLOT_AVG_1;
  ADCSlotConfig.ePrecisionMode  = AM_HAL_ADC_SLOT_10BIT;
  ADCSlotConfig.eChannel        = AM_HAL_ADC_SLOT_CHSEL_SE0;
  ADCSlotConfig.bWindowCompare  = false;
  ADCSlotConfig.bEnabled        = true;
  if (AM_HAL_STATUS_SUCCESS != am_hal_adc_configure_slot(g_ADCHandle, 0, &ADCSlotConfig))
  {
    Serial.println("Error - configuring ADC Slot 0 failed.\n");
  }

  // Enable the ADC.
  if (AM_HAL_STATUS_SUCCESS != am_hal_adc_enable(g_ADCHandle))
  {
    Serial.println("Error - enabling ADC failed.\n");
  }

  Serial.println("ADC config complete");
}
