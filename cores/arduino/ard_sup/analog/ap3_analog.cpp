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
#include "ap3_analog.h"

uint16_t _analogBits = 10; //10-bit by default

uint16_t analogRead(uint8_t padNumber)
{

    uint32_t ui32IntMask;
    am_hal_adc_sample_t Sample;
    uint32_t ui32NumSamples = 1;

    am_hal_adc_sw_trigger(g_ADCHandle);

    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_samples_read(g_ADCHandle,
                                                         false,
                                                         NULL,
                                                         &ui32NumSamples,
                                                         &Sample))
    {
        am_util_stdio_printf("Error - ADC sample read failed.\n");
    }

    //Shift result depending on resolution
    if (_analogBits <= 14)
    {
        return (Sample.ui32Sample >> (14 - _analogBits));
    }
    else
    {
        return (Sample.ui32Sample << (_analogBits - 14)); //Pad with zeros
    }
}

//Apollo3 is capapble of 14-bit ADC but Arduino defaults to 10-bit
//This modifies the global var that controls what is returned from an analogRead()
void analogReadResolution(uint8_t bits)
{
    _analogBits = bits;
}

//Set pad number to analog input
ap3_err_t analogSetup(ap3_gpio_pad_t padNumber)
{
    ap3_err_t retval = AP3_ERR;

    uint8_t funcsel = 0;
    am_hal_gpio_pincfg_t pincfg = INPUT;

    retval = ap3_analog_pad_funcsel(ap3_gpio_pin2pad(padNumber), &funcsel);
    if (retval != AP3_OK)
    {
        return retval;
    }
    pincfg.uFuncSel = funcsel; // set the proper function select option for this instance/pin/type combination
    pinMode(padNumber, pincfg, &retval);
}

//Given pad number, assign ADC function
ap3_err_t ap3_analog_pad_funcsel(ap3_gpio_pad_t padNumber, uint8_t *funcsel)
{
    ap3_err_t retval = AP3_ERR;

    const ap3_analog_pad_map_elem_t *map = NULL;
    uint8_t map_len = 0;

    map = ap3_analog_map;
    map_len = AP3_ANALOG_PADS;

    for (uint16_t indi = 0; indi < map_len; indi++)
    {
        if (map[indi].pad == padNumber)
        {
            *funcsel = map[indi].funcsel;
            return AP3_OK;
        }
    }
    return retval;

invalid_args:
    retval = AP3_INVALID_ARG;
    return retval;
}

ap3_err_t adc_config(ap3_gpio_pad_t padNumber)
{
    ap3_err_t retval = AP3_ERR;

    am_hal_adc_config_t ADCConfig;
    am_hal_adc_slot_config_t ADCSlotConfig;

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
    ADCConfig.eClockMode = AM_HAL_ADC_CLKMODE_LOW_POWER;
    ADCConfig.ePowerMode = AM_HAL_ADC_LPMODE0;
    ADCConfig.eRepeat = AM_HAL_ADC_SINGLE_SCAN;
    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_configure(g_ADCHandle, &ADCConfig))
    {
        //Serial.println("Error - configuring ADC failed.\n");
        return AP3_ERR;
    }

    // Set up an ADC slot
    ADCSlotConfig.eMeasToAvg = AM_HAL_ADC_SLOT_AVG_1;
    ADCSlotConfig.ePrecisionMode = AM_HAL_ADC_SLOT_14BIT;

    //ADCSlotConfig.eChannel = AM_HAL_ADC_SLOT_CHSEL_SE0;

    //Look up adc channel based on pad number
    uint8_t indi;
    for (indi = 0; indi < AP3_ANALOG_PADS; indi++)
    {
        if (ap3_analog_channel_map[indi].pad == padNumber)
        {
            ADCSlotConfig.eChannel = ap3_analog_channel_map[indi].eChannel;
            break;
        }
    }
    if (indi == AP3_ANALOG_PADS)
    {
        return AP3_ERR;
    }

    ADCSlotConfig.bWindowCompare = false;
    ADCSlotConfig.bEnabled = true;
    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_configure_slot(g_ADCHandle, 0, &ADCSlotConfig))
    {
        //Serial.println("Error - configuring ADC Slot 0 failed.\n");
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