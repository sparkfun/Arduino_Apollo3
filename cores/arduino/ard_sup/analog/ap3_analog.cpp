/*
Copyright (c) 2019 SparkFun Electronics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
    TODO
    analogReference
*/

#include "ap3_analog.h"

// Define the clock source and frequency to use for
// PWM generation.
// Chose 12 MHz to allow maximal resolution with a
// target maximum width of 2ms (for RC servos)
// 1/12MHz = 0.083 uS per LSB.
// 2ms/0.083us = 24000 LSB for 2 ms wide pulse

/* *** DISCLAIMER *** */
/*
    Despite all the talk about servos above ^ the analogWrite()
    function should not be used with servos. Try servoWrite()
    instead or maybe we can even go ahead and support the Servo library
*/

#define AP3_ANALOG_CLK
#define AP3_ANALOG_FREQ 12000000
#define AP3_ANALOG_FRAME_PERIOD 24000

//*****************************************************************************
//
//  Tables copied from am_hal_ctimer.c because they are declared as static within
//  that file, but they would be useful here too.
//
//  Lookup tables used by am_hal_ctimer_output_config().
//
//  CTx_tbl[] relates the padnum and pad funcsel based on a given CTx.
//  Valid pads for CTx are: 4-7, 11-13, 18-19, 22-33, 35, 37, 39, 42-49.
//
//  outcfg_tbl[] contains attributes of the 4 output signal types for each
//  of the 32 CTx signals. Therefore it is indexed by CTnumber 0-31.
//  This table provides only the non-common OUTCFG attributes (2-5, other
//  settings are shown below).
//  OUTCFG 0 = Force output to 0.
//  OUTCFG 1 = Force output to 1.
//  OUTCFG 6 = A6OUT2.
//  OUTCFG 7 = A7OUT2.
//
//*****************************************************************************
#define CTXPADNUM(ctx) ((CTx_tbl[ctx] >> 0) & 0x3f)
#define CTXPADFNC(ctx) ((CTx_tbl[ctx] >> 8) & 0x7)
#define CTX(pad, fn) ((fn << 8) | (pad << 0))
static const uint16_t CTx_tbl[32] =
    {
        CTX(12, 2), CTX(25, 2), CTX(13, 2), CTX(26, 2), CTX(18, 2), // 0 - 4
        CTX(27, 2), CTX(19, 2), CTX(28, 2), CTX(5, 7), CTX(29, 2),  // 5 - 9
        CTX(6, 5), CTX(30, 2), CTX(22, 2), CTX(31, 2), CTX(23, 2),  // 10 - 14
        CTX(32, 2), CTX(42, 2), CTX(4, 6), CTX(43, 2), CTX(7, 7),   // 15 - 19
        CTX(44, 2), CTX(24, 5), CTX(45, 2), CTX(33, 6), CTX(46, 2), // 20 - 24
        CTX(39, 2), CTX(47, 2), CTX(35, 5), CTX(48, 2), CTX(37, 7), // 25 - 29
        CTX(49, 2), CTX(11, 2)                                      // 30 - 31
};

#define OUTC(timB, timN, N2) ((N2 << 4) | (timB << 3) | (timN << 0))
#define OUTCTIMN(ctx, n) (outcfg_tbl[ctx][n] & (0x7 << 0))
#define OUTCTIMB(ctx, n) (outcfg_tbl[ctx][n] & (0x1 << 3))
#define OUTCO2(ctx, n) (outcfg_tbl[ctx][n] & (0x1 << 4))
static const uint8_t outcfg_tbl[32][4] =
    {
        {OUTC(0, 0, 0), OUTC(1, 2, 1), OUTC(0, 5, 1), OUTC(0, 6, 0)}, // CTX0:  A0OUT,  B2OUT2, A5OUT2, A6OUT
        {OUTC(0, 0, 1), OUTC(0, 0, 0), OUTC(0, 5, 0), OUTC(1, 7, 1)}, // CTX1:  A0OUT2, A0OUT,  A5OUT,  B7OUT2
        {OUTC(1, 0, 0), OUTC(1, 1, 1), OUTC(1, 6, 1), OUTC(0, 7, 0)}, // CTX2:  B0OUT,  B1OUT2, B6OUT2, A7OUT
        {OUTC(1, 0, 1), OUTC(1, 0, 0), OUTC(0, 1, 0), OUTC(0, 6, 0)}, // CTX3:  B0OUT2, B0OUT,  A1OUT,  A6OUT
        {OUTC(0, 1, 0), OUTC(0, 2, 1), OUTC(0, 5, 1), OUTC(1, 5, 0)}, // CTX4:  A1OUT,  A2OUT2, A5OUT2, B5OUT
        {OUTC(0, 1, 1), OUTC(0, 1, 0), OUTC(1, 6, 0), OUTC(0, 7, 0)}, // CTX5:  A1OUT2, A1OUT,  B6OUT,  A7OUT
        {OUTC(1, 1, 0), OUTC(0, 1, 0), OUTC(1, 5, 1), OUTC(1, 7, 0)}, // CTX6:  B1OUT,  A1OUT,  B5OUT2, B7OUT
        {OUTC(1, 1, 1), OUTC(1, 1, 0), OUTC(1, 5, 0), OUTC(0, 7, 0)}, // CTX7:  B1OUT2, B1OUT,  B5OUT,  A7OUT
        {OUTC(0, 2, 0), OUTC(0, 3, 1), OUTC(0, 4, 1), OUTC(1, 6, 0)}, // CTX8:  A2OUT,  A3OUT2, A4OUT2, B6OUT
        {OUTC(0, 2, 1), OUTC(0, 2, 0), OUTC(0, 4, 0), OUTC(1, 0, 0)}, // CTX9:  A2OUT2, A2OUT,  A4OUT,  B0OUT
        {OUTC(1, 2, 0), OUTC(1, 3, 1), OUTC(1, 4, 1), OUTC(0, 6, 0)}, // CTX10: B2OUT,  B3OUT2, B4OUT2, A6OUT
        {OUTC(1, 2, 1), OUTC(1, 2, 0), OUTC(1, 4, 0), OUTC(1, 5, 1)}, // CTX11: B2OUT2, B2OUT,  B4OUT,  B5OUT2
        {OUTC(0, 3, 0), OUTC(1, 1, 0), OUTC(1, 0, 1), OUTC(1, 6, 1)}, // CTX12: A3OUT,  B1OUT,  B0OUT2, B6OUT2
        {OUTC(0, 3, 1), OUTC(0, 3, 0), OUTC(0, 6, 0), OUTC(1, 4, 1)}, // CTX13: A3OUT2, A3OUT,  A6OUT,  B4OUT2
        {OUTC(1, 3, 0), OUTC(1, 1, 0), OUTC(1, 7, 1), OUTC(0, 7, 0)}, // CTX14: B3OUT,  B1OUT,  B7OUT2, A7OUT
        {OUTC(1, 3, 1), OUTC(1, 3, 0), OUTC(0, 7, 0), OUTC(0, 4, 1)}, // CTX15: B3OUT2, B3OUT,  A7OUT,  A4OUT2
        {OUTC(0, 4, 0), OUTC(0, 0, 0), OUTC(0, 0, 1), OUTC(1, 3, 1)}, // CTX16: A4OUT,  A0OUT,  A0OUT2, B3OUT2
        {OUTC(0, 4, 1), OUTC(1, 7, 0), OUTC(0, 4, 0), OUTC(0, 1, 1)}, // CTX17: A4OUT2, B7OUT,  A4OUT,  A1OUT2
        {OUTC(1, 4, 0), OUTC(1, 0, 0), OUTC(0, 0, 0), OUTC(0, 3, 1)}, // CTX18: B4OUT,  B0OUT,  A0OUT,  A3OUT2
        {OUTC(1, 4, 1), OUTC(0, 2, 0), OUTC(1, 4, 0), OUTC(1, 1, 1)}, // CTX19: B4OUT2, A2OUT,  B4OUT,  B1OUT2
        {OUTC(0, 5, 0), OUTC(0, 1, 0), OUTC(0, 1, 1), OUTC(1, 2, 1)}, // CTX20: A5OUT,  A1OUT,  A1OUT2, B2OUT2
        {OUTC(0, 5, 1), OUTC(0, 1, 0), OUTC(1, 5, 0), OUTC(0, 0, 1)}, // CTX21: A5OUT2, A1OUT,  B5OUT,  A0OUT2
        {OUTC(1, 5, 0), OUTC(0, 6, 0), OUTC(0, 1, 0), OUTC(0, 2, 1)}, // CTX22: B5OUT,  A6OUT,  A1OUT,  A2OUT2
        {OUTC(1, 5, 1), OUTC(0, 7, 0), OUTC(0, 5, 0), OUTC(1, 0, 1)}, // CTX23: B5OUT2, A7OUT,  A5OUT,  B0OUT2
        {OUTC(0, 6, 0), OUTC(0, 2, 0), OUTC(0, 1, 0), OUTC(1, 1, 1)}, // CTX24: A6OUT,  A2OUT,  A1OUT,  B1OUT2
        {OUTC(1, 4, 1), OUTC(1, 2, 0), OUTC(0, 6, 0), OUTC(0, 2, 1)}, // CTX25: B4OUT2, B2OUT,  A6OUT,  A2OUT2
        {OUTC(1, 6, 0), OUTC(1, 2, 0), OUTC(0, 5, 0), OUTC(0, 1, 1)}, // CTX26: B6OUT,  B2OUT,  A5OUT,  A1OUT2
        {OUTC(1, 6, 1), OUTC(0, 1, 0), OUTC(1, 6, 0), OUTC(1, 2, 1)}, // CTX27: B6OUT2, A1OUT,  B6OUT,  B2OUT2
        {OUTC(0, 7, 0), OUTC(0, 3, 0), OUTC(0, 5, 1), OUTC(1, 0, 1)}, // CTX28: A7OUT,  A3OUT,  A5OUT2, B0OUT2
        {OUTC(1, 5, 1), OUTC(0, 1, 0), OUTC(0, 7, 0), OUTC(0, 3, 1)}, // CTX29: B5OUT2, A1OUT,  A7OUT,  A3OUT2
        {OUTC(1, 7, 0), OUTC(1, 3, 0), OUTC(0, 4, 1), OUTC(0, 0, 1)}, // CTX30: B7OUT,  B3OUT,  A4OUT2, A0OUT2
        {OUTC(1, 7, 1), OUTC(0, 6, 0), OUTC(1, 7, 0), OUTC(1, 3, 1)}, // CTX31: B7OUT2, A6OUT,  B7OUT,  B3OUT2
};

uint16_t _analogBits = 10;    //10-bit by default
uint8_t _analogWriteBits = 8; // 8-bit by default for writes
uint8_t _servoWriteBits = 8;  // 8-bit by default for writes

uint16_t analogRead(uint8_t pinNumber)
{
    uint32_t ui32IntMask;
    am_hal_adc_sample_t Sample;
    uint32_t ui32NumSamples = 1;

    uint8_t padNumber = ap3_gpio_pin2pad(pinNumber);

    //Look up configuration status based on pad number
    uint8_t indi;
    for (indi = 0; indi < AP3_ANALOG_CHANNELS; indi++)
    {
        if (ap3_analog_configure_map[indi].pad == padNumber)
        {
            if (ap3_analog_configure_map[indi].isAnalog == false)
            {
                if (ap3_set_pin_to_analog(pinNumber) != AP3_OK)
                {
                    //Serial.println("Error - set pin to analog");
                    return 0; //Error
                }
                ap3_analog_configure_map[indi].isAnalog = true;
            }
            break;
        }
    }
    if (indi == AP3_ANALOG_CHANNELS)
    {
        //Serial.println("Error - pin set to analog not found");
        return 0; //Error
    }

    ap3_change_channel(padNumber); //Point ADC channel at this pad

    // Clear the ADC interrupt.
    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_interrupt_clear(g_ADCHandle, ui32IntMask))
    {
        //Serial.println("Error clearing ADC interrupt status");
        return 0; //Error
    }

    am_hal_adc_sw_trigger(g_ADCHandle);

    //Wait for interrupt
    while (1)
    {
        // Read the interrupt status.
        if (AM_HAL_STATUS_SUCCESS != am_hal_adc_interrupt_status(g_ADCHandle, &ui32IntMask, false))
        {
            //Serial.println("Error reading ADC interrupt status");
            return 0; //Error
        }
        if (ui32IntMask & AM_HAL_ADC_INT_CNVCMP)
            break;
    }

    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_samples_read(g_ADCHandle,
                                                         false,
                                                         NULL,
                                                         &ui32NumSamples,
                                                         &Sample))
    {
        //Serial.println("Error - ADC sample read failed.\n");
        return 0; //Error
    }

    uint32_t result = Sample.ui32Sample;

    //Shift result depending on resolution
    if (_analogBits <= 14)
    {
        return (result >> (14 - _analogBits));
    }
    else
    {
        return (result << (_analogBits - 14)); //Pad with zeros
    }
}

//Power down ADC. Comes from adc_lpmode2.c example from Ambiq SDK
bool power_adc_disable()
{
    // Disable the ADC.
    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_disable(g_ADCHandle))
    {
        //am_util_stdio_printf("Error - disable ADC failed.\n");
        return (false);
    }

    // Enable the ADC power domain.
    if (AM_HAL_STATUS_SUCCESS != am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_ADC))
    {
        //am_util_stdio_printf("Error - disabling the ADC power domain failed.\n");
        return (false);
    }

    // Deinitialize the ADC
    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_deinitialize(g_ADCHandle))
    {
        //am_util_stdio_printf("Error - return of the ADC instance failed.\n");
        return (false);
    }

    g_ADCHandle = NULL;
    return (true);
}

//Apollo3 is capapble of 14-bit ADC but Arduino defaults to 10-bit
//This modifies the global var that controls what is returned from analogRead()
void analogReadResolution(uint8_t bits)
{
    _analogBits = bits;
}

ap3_err_t ap3_adc_setup()
{
    ap3_err_t retval = AP3_ERR;

    am_hal_adc_config_t ADCConfig;

    // Initialize the ADC and get the handle.
    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_initialize(0, &g_ADCHandle))
    {
        //Serial.println("Error - reservation of the ADC instance failed.\n");
        return AP3_ERR;
    }

    // Power on the ADC.
    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_power_control(g_ADCHandle,
                                                          AM_HAL_SYSCTRL_WAKE,
                                                          false))
    {
        //Serial.println("Error - ADC power on failed.\n");
        return AP3_ERR;
    }

    // Set up the ADC configuration parameters. These settings are reasonable
    // for accurate measurements at a low sample rate.
    ADCConfig.eClock = AM_HAL_ADC_CLKSEL_HFRC;
    ADCConfig.ePolarity = AM_HAL_ADC_TRIGPOL_RISING;
    ADCConfig.eTrigger = AM_HAL_ADC_TRIGSEL_SOFTWARE;
    ADCConfig.eReference = AM_HAL_ADC_REFSEL_INT_2P0;
    ADCConfig.eClockMode = AM_HAL_ADC_CLKMODE_LOW_LATENCY;
    ADCConfig.ePowerMode = AM_HAL_ADC_LPMODE0;
    ADCConfig.eRepeat = AM_HAL_ADC_SINGLE_SCAN;
    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_configure(g_ADCHandle, &ADCConfig))
    {
        //Serial.println("Error - configuring ADC failed.\n");
        return AP3_ERR;
    }

    // Enable the ADC.
    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_enable(g_ADCHandle))
    {
        //Serial.println("Error - enabling ADC failed.\n");
        return AP3_ERR;
    }

    return AP3_OK;
}

//Set function of pin to analog input
//TODO Support differential pairs 0/1
ap3_err_t ap3_set_pin_to_analog(uint8_t pinNumber)
{
    ap3_err_t retval = AP3_ERR;

    uint8_t funcsel = 0;
    am_hal_gpio_pincfg_t pincfg = INPUT;

    retval = ap3_analog_pad_funcsel(ap3_gpio_pin2pad(pinNumber), &funcsel);
    if (retval != AP3_OK)
    {
        return retval;
    }
    pincfg.uFuncSel = funcsel; // set the proper function select option for this instance/pin/type combination
    pinMode(pinNumber, pincfg, &retval);
    return retval;
}

//Given pin number, assign ADC function
ap3_err_t ap3_analog_pad_funcsel(ap3_gpio_pad_t padNumber, uint8_t *funcsel)
{
    ap3_err_t retval = AP3_ERR;

    for (uint16_t indi = 0; indi < AP3_ANALOG_PADS; indi++)
    {
        if (ap3_analog_map[indi].pad == padNumber)
        {
            *funcsel = ap3_analog_map[indi].funcsel;
            return AP3_OK;
        }
    }
    return retval;
}

ap3_err_t ap3_change_channel(uint8_t padNumber)
{
    am_hal_adc_slot_config_t ADCSlotConfig;

    // Set up an ADC slot
    ADCSlotConfig.eMeasToAvg = AM_HAL_ADC_SLOT_AVG_1;
    ADCSlotConfig.ePrecisionMode = AM_HAL_ADC_SLOT_14BIT;

    //Look up adc channel based on pad number
    uint8_t indi;
    for (indi = 0; indi < AP3_ANALOG_CHANNELS; indi++)
    {
        if (ap3_analog_channel_map[indi].pad == padNumber)
        {
            ADCSlotConfig.eChannel = ap3_analog_channel_map[indi].eChannel;
            break;
        }
    }
    if (indi == AP3_ANALOG_CHANNELS)
    {
        //Serial.println("Error - channel not found");
        return AP3_ERR;
    }

    ADCSlotConfig.bWindowCompare = false;
    ADCSlotConfig.bEnabled = true;

    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_configure_slot(g_ADCHandle, 0, &ADCSlotConfig))
    {
        //Serial.println("Error - configuring ADC Slot 0 failed.\n");
        return AP3_ERR;
    }
}


bool ap3_pwm_is_running(uint32_t ui32TimerNumber, uint32_t ui32TimerSegment){
    volatile uint32_t *pui32ConfigReg;
    bool is_enabled = false;

    //
    // Find the correct control register.
    //
    pui32ConfigReg = (uint32_t*)CTIMERADDRn(CTIMER, ui32TimerNumber, CTRL0);

    //
    // Begin critical section while config registers are read and modified.
    //
    AM_CRITICAL_BEGIN

    //
    // Read the current value.
    //
    uint32_t ui32ConfigVal = *pui32ConfigReg;

    //
    // Check the "enable bit"
    //
    if( ui32ConfigVal & (CTIMER_CTRL0_TMRA0EN_Msk | CTIMER_CTRL0_TMRB0EN_Msk) ){
        is_enabled = true;
    }

    //
    // Done with critical section.
    //
    AM_CRITICAL_END

    return is_enabled;
}


void ap3_pwm_wait_for_pulse(uint32_t timer, uint32_t segment, uint32_t output, uint32_t margin){

    volatile uint32_t *pui32CompareReg;
    volatile uint32_t ctimer_val;
    uint32_t cmpr0;

    // Only wait if the ctimer is running to avoid a deadlock
    if( ap3_pwm_is_running( timer, segment) ){

        // Get the comapre register address
        if( segment == AM_HAL_CTIMER_TIMERA ){
            if( output == AM_HAL_CTIMER_OUTPUT_NORMAL ){
                pui32CompareReg = (uint32_t*)CTIMERADDRn(CTIMER, timer, CMPRA0);
            }else{
                pui32CompareReg = (uint32_t*)CTIMERADDRn(CTIMER, timer, CMPRAUXA0);
            }
        }else{
            if( output == AM_HAL_CTIMER_OUTPUT_NORMAL ){
                pui32CompareReg = (uint32_t*)CTIMERADDRn(CTIMER, timer, CMPRB0);
            }else{
                pui32CompareReg = (uint32_t*)CTIMERADDRn(CTIMER, timer, CMPRAUXB0);
            }
        }

        // Get the compare value
        cmpr0 = ((uint32_t)(*(pui32CompareReg)) & 0x0000FFFF);
        
        // Wait for the timer value to be less than the compare value so that it is safe to change
        ctimer_val = am_hal_ctimer_read( timer, segment);
        while( (ctimer_val + 0) > cmpr0 ){
            ctimer_val = am_hal_ctimer_read( timer, segment);
        }
    }
}

//**********************************************
// ap3_pwm_output
// - This function allows you to specify an arbitrary pwm output signal with a given frame width (fw) and time high (th).
// - Due to contraints of the hardware th must be lesser than fw by at least 2.
// - Furthermore fw must be at least 3 to see any high pulses
//
// This causes the most significant deviations for small values of fw. For example:
//
// th = 0, fw = 2 -->   0% duty cycle as expected
// th = 1, fw = 2 --> 100% duty cycle --- expected 50%, so 50% error ---
// th = 2, fw = 2 --> 100% duty cycle as expected
//
// th = 0, fw = 3 -->   0% duty cycle as expected
// th = 1, fw = 3 -->  33% duty cycle as expected
// th = 2, fw = 3 --> 100% duty cycle --- expected 66%, so 33% error ---
// th = 3, fw = 3 --> 100% duty cycle as expected
//
// th = 0, fw = 4 -->   0% duty cycle as expected
// th = 1, fw = 4 -->  25% duty cycle as expected
// th = 2, fw = 4 -->  50% duty cycle as expected
// th = 3, fw = 4 --> 100% duty cycle --- expected 75%, so 25% error ---
// th = 4, fw = 4 --> 100% duty cycle as expected
//
// ...
//
// Then we conclude that for the case th == (fw - 1) the duty cycle will be 100% and
// the percent error from the expected duty cycle will be 100/fw
//**********************************************

ap3_err_t ap3_pwm_output(uint8_t pin, uint32_t th, uint32_t fw, uint32_t clk)
{
    // handle configuration, if necessary
    ap3_err_t retval = AP3_OK;

    if (fw > 0)
    { // reduce fw so that the user's desired value is the period
        fw--;
    }

    ap3_gpio_pad_t pad = ap3_gpio_pin2pad(pin);
    if ((pad == AP3_GPIO_PAD_UNUSED) || (pad >= AP3_GPIO_MAX_PADS))
    {
        return AP3_INVALID_ARG;
    }

    uint32_t timer = 0;
    uint32_t segment = AM_HAL_CTIMER_TIMERA;
    uint32_t output = AM_HAL_CTIMER_OUTPUT_NORMAL;

    uint8_t ctx = 0;
    for (ctx = 0; ctx < 32; ctx++)
    {
        if (CTXPADNUM(ctx) == pad)
        {
            break;
        }
    }
    if (ctx >= 32)
    {
        return AP3_ERR; // could not find pad in CTx table
    }
    // Now use CTx index to get configuration information

    // Now, for the given pad, determine the above values
    if ((pad == 39) || (pad == 37))
    {
        // pads 39 and 37 must be handled differently to avoid conflicting with other pins
        if (pad == 39)
        {
            // 39
            timer = 6;
            segment = AM_HAL_CTIMER_TIMERA;
            output = AM_HAL_CTIMER_OUTPUT_SECONDARY;
        }
        else
        {
            // 37
            timer = 7;
            segment = AM_HAL_CTIMER_TIMERA;
            output = AM_HAL_CTIMER_OUTPUT_SECONDARY;
        }
    }
    else
    { // Use the 0th index of the outcfg_tbl to select the functions
        timer = OUTCTIMN(ctx, 0);
        if (OUTCTIMB(ctx, 0))
        {
            segment = AM_HAL_CTIMER_TIMERB;
        }
        if (OUTCO2(ctx, 0))
        {
            output = AM_HAL_CTIMER_OUTPUT_SECONDARY;
        }
    }

    // Ensure that th is not greater than the fw
    if (th > fw)
    {
        th = fw;
    }

    // Test for AM_HAL_CTIMER_OUTPUT_FORCE0 or AM_HAL_CTIMER_OUTPUT_FORCE1
    if ((th == 0) || (fw == 0))
    {
        output = AM_HAL_CTIMER_OUTPUT_FORCE0;
    }
    else if (th == fw)
    {
        output = AM_HAL_CTIMER_OUTPUT_FORCE1;
    }

    // Wait until after high pulse to change the state (avoids inversion)
    ap3_pwm_wait_for_pulse( timer, segment, output, 10);

    // Configure the pin
    am_hal_ctimer_output_config(timer,
                                segment,
                                pad,
                                output,
                                AM_HAL_GPIO_PIN_DRIVESTRENGTH_12MA); //

    // Configure the repeated pulse mode with our clock source
    am_hal_ctimer_config_single(timer,
                                segment,
                                // (AM_HAL_CTIMER_FN_PWM_REPEAT | AP3_ANALOG_CLK | AM_HAL_CTIMER_INT_ENABLE) );
                                (AM_HAL_CTIMER_FN_PWM_REPEAT | clk));

    // If this pad uses secondary output:
    if (output == AM_HAL_CTIMER_OUTPUT_SECONDARY)
    {
        // Need to explicitly enable compare registers 2/3
        uint32_t *pui32ConfigReg = NULL;
        pui32ConfigReg = (uint32_t *)CTIMERADDRn(CTIMER, timer, AUX0);
        uint32_t ui32WriteVal = AM_REGVAL(pui32ConfigReg);
        uint32_t ui32ConfigVal = (1 << CTIMER_AUX0_TMRA0EN23_Pos); // using CTIMER_AUX0_TMRA0EN23_Pos because for now this number is common to all CTimer instances
        if (segment == AM_HAL_CTIMER_TIMERB)
        {
            ui32ConfigVal = ((ui32ConfigVal & 0xFFFF) << 16);
        }
        ui32WriteVal = (ui32WriteVal & ~(segment)) | ui32ConfigVal;
        AM_REGVAL(pui32ConfigReg) = ui32WriteVal;

        // then set the duty cycle with the 'aux' function
        am_hal_ctimer_aux_period_set(timer, segment, fw, th);
    }
    else
    {
        // Otherwise simply set the primary duty cycle
        am_hal_ctimer_period_set(timer, segment, fw, th);
    }

    am_hal_ctimer_start(timer, segment);

    return AP3_OK;
}

ap3_err_t analogWriteResolution(uint8_t res)
{
    if (res > 16)
    {
        _analogWriteBits = 16; // max out the resolution when this happens
        return AP3_ERR;
    }
    _analogWriteBits = res;
    return AP3_OK;
}

ap3_err_t analogWrite(uint8_t pin, uint32_t val)
{
    // Determine the high time based on input value and the current resolution setting
    uint32_t fw = 0xFFFF; // Choose the frame width in clock periods (32767 -> ~ 180 Hz)
    if (val == ((0x01 << _analogWriteBits) - 1))
    {
        val = fw; // Enable FORCE1
    }
    else
    {
        val <<= (16 - _analogWriteBits); // Shift over the value to fill available resolution
    }
    uint32_t clk = AM_HAL_CTIMER_HFRC_12MHZ; // Use an Ambiq HAL provided value to select which clock

    return ap3_pwm_output(pin, val, fw, clk);
}

ap3_err_t servoWriteResolution(uint8_t res)
{
    if (res > 16)
    {
        _servoWriteBits = 16; // max out the resolution when this happens
        return AP3_ERR;
    }
    _servoWriteBits = res;
    return AP3_OK;
}

ap3_err_t servoWrite(uint8_t pin, uint32_t val)
{
    // Determine the high time based on input value and the current resolution setting
    uint32_t fsv = (0x01 << _servoWriteBits); // full scale value for the current resolution setting
    val = val % fsv;                          // prevent excess
    uint32_t clk = AM_HAL_CTIMER_HFRC_3MHZ;   // Using 3 MHz to get fine-grained control up to 20 ms wide
    uint32_t fw = 60000;                      // 20 ms wide frame
    uint32_t max = 6000;                      // max width of RC pwm pulse is 2 ms or 6000 counts
    uint32_t min = 3000;                      // min width of RC pwm pulse is 1 ms or 3000 counts
    uint32_t th = (uint32_t)(((max - min) * val) / fsv) + min;

    return ap3_pwm_output(pin, th, fw, clk);
}

ap3_err_t tone(uint8_t pin, uint32_t freq)
{
    uint32_t clk = AM_HAL_CTIMER_HFRC_12MHZ;

    uint32_t fw = 0;
    if (freq > 0)
    {
        // Determine the frame width based on input freq
        fw = 12000000 / freq;
    }
    uint32_t th = fw / 2; // 50% time high

    return ap3_pwm_output(pin, th, fw, clk);
}
ap3_err_t tone(uint8_t pin, uint32_t freq, uint32_t duration)
{
    ap3_err_t status = AP3_OK;
    status = tone(pin, freq);
    if (status != AP3_OK)
    {
        return (status);
    }

    uint32_t startTime = millis();

    while (millis() - startTime < duration)
        ;

    status = noTone(pin);
    return (status);
}

ap3_err_t noTone(uint8_t pin)
{
    return tone(pin, 0);
}