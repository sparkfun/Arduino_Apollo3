/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#define PinMode Arduino_PinMode
#include "core-api/api/Common.h"
#undef PinMode

#include "core-extend/Common.h"
#include "sdk/core-extend/Common.h"

#include "sdk/core-extend/AnalogInternal.h"

#define AP3_ANALOG_CLK_SRC AM_HAL_CTIMER_HFRC_12MHZ
#define AP3_ANALOG_CLK_FREQ (12000000)
#define AP3_ANALOG_FRAME_PERIOD (24000)
#define AP3_MAX_ANALOG_WRITE_WIDTH (0x0000FFFF)
#define AP3_MIN_ANALOG_WRITE_WIDTH (0x03)
#define AP3_ANALOG_WRITE_RESOLUTION_MAX (16)
#define AP3_ANALOG_WRITE_RESOLUTION_MIN (1)
#define AP3_ANALOG_READ_RESOLUTION_MAX (16)
#define AP3_ANALOG_READ_RESOLUTION_MIN (1)
#define AP3_ADC_RESOLUTION (14)

int ap3_analog_read(ap3_adc_channel_config_t* config);
ap3_err_t ap3_pwm_output(ap3_gpio_pad_t pad, uint32_t th, uint32_t fw, uint32_t clk);

static uint8_t _analogReadResolution = 10;
static uint8_t _servoWriteResolution = 8;
static uint8_t _analogWriteResolution = 8;
static uint32_t _analogWriteWidth = 0x0000FFFF;

static void* g_ADCHandle;
bool adc_initialized = false;

ap3_adc_channel_config_t ap3_adc_channel_configs[] = {
    {11,    AP3_ADC_INTERNAL_CHANNELS_NUM,      AM_HAL_ADC_SLOT_CHSEL_SE2,      AM_HAL_PIN_11_ADCSE2,       },
    {12,    AP3_ADC_INTERNAL_CHANNELS_NUM,      AM_HAL_ADC_SLOT_CHSEL_SE9,      AM_HAL_PIN_12_ADCD0NSE9,    },
    {13,    AP3_ADC_INTERNAL_CHANNELS_NUM,      AM_HAL_ADC_SLOT_CHSEL_SE8,      AM_HAL_PIN_13_ADCD0PSE8,    },
    {16,    AP3_ADC_INTERNAL_CHANNELS_NUM,      AM_HAL_ADC_SLOT_CHSEL_SE0,      AM_HAL_PIN_16_ADCSE0,       },
    {29,    AP3_ADC_INTERNAL_CHANNELS_NUM,      AM_HAL_ADC_SLOT_CHSEL_SE1,      AM_HAL_PIN_29_ADCSE1,       },
    {31,    AP3_ADC_INTERNAL_CHANNELS_NUM,      AM_HAL_ADC_SLOT_CHSEL_SE3,      AM_HAL_PIN_31_ADCSE3,       },
    {32,    AP3_ADC_INTERNAL_CHANNELS_NUM,      AM_HAL_ADC_SLOT_CHSEL_SE4,      AM_HAL_PIN_32_ADCSE4,       },
    {33,    AP3_ADC_INTERNAL_CHANNELS_NUM,      AM_HAL_ADC_SLOT_CHSEL_SE5,      AM_HAL_PIN_33_ADCSE5,       },
    {34,    AP3_ADC_INTERNAL_CHANNELS_NUM,      AM_HAL_ADC_SLOT_CHSEL_SE6,      AM_HAL_PIN_34_ADCSE6,       },
    {35,    AP3_ADC_INTERNAL_CHANNELS_NUM,      AM_HAL_ADC_SLOT_CHSEL_SE7,      AM_HAL_PIN_35_ADCSE7,       },
    { 0,    AP3_ADC_INTERNAL_CHANNELS_DIFF0,    AM_HAL_ADC_SLOT_CHSEL_DF0,      0,                          },
    { 0,    AP3_ADC_INTERNAL_CHANNELS_DIFF1,    AM_HAL_ADC_SLOT_CHSEL_DF1,      0,                          },
    { 0,    AP3_ADC_INTERNAL_CHANNELS_TEMP,     AM_HAL_ADC_SLOT_CHSEL_TEMP,     0,                          },
    { 0,    AP3_ADC_INTERNAL_CHANNELS_VCC_DIV3, AM_HAL_ADC_SLOT_CHSEL_BATT,     0,                          },
    { 0,    AP3_ADC_INTERNAL_CHANNELS_VSS,      AM_HAL_ADC_SLOT_CHSEL_VSS,      0,                          },
};

float getTempDegC( void ) {
    const float v_ref = 2.0;
    uint16_t counts = analogReadTemp();

    //
    // Convert and scale the temperature.
    // Temperatures are in Fahrenheit range -40 to 225 degrees.
    // Voltage range is 0.825V to 1.283V
    // First get the ADC voltage corresponding to temperature.
    //
    float volts = ((float)counts) * v_ref / ((float)(pow(2, _analogReadResolution)));

    float fVT[3];
    fVT[0] = volts;
    fVT[1] = 0.0f;
    fVT[2] = -123.456;
    uint32_t ui32Retval = am_hal_adc_control(g_ADCHandle, AM_HAL_ADC_REQ_TEMP_CELSIUS_GET, fVT);
    MBED_ASSERT(ui32Retval == AM_HAL_STATUS_SUCCESS);

    return fVT[1]; // Get the temperature
}

float getTempDegF( void ) {
    float temp_deg_c = getTempDegC();
    return ((9.0f/5.0f)*temp_deg_c) + 32.0f;
}

float getVCCV( void ){
    return ((float)analogReadVCCDiv3() * 2.0) / 16384.0;
}

int indexAnalogRead(pin_size_t index){
    // todo: replace with mbed "AnalogIn" functionality
    pin_size_t pinNumber = pinNameByIndex(index);
    if(pinNumber == (pin_size_t)NC){ return 0; }
    ap3_adc_channel_config_t* config = NULL;
    for(size_t idx = 0; idx < (sizeof(ap3_adc_channel_configs)/sizeof(ap3_adc_channel_config_t)); idx++){
        if(ap3_adc_channel_configs[idx].pinNumber == pinNumber){
            config = (ap3_adc_channel_configs + idx);
            break;
        }
    }
    if(!config){ return 0; }
    return ap3_analog_read(config);
}

int analogReadDiff( uint32_t instance ){
    ap3_adc_channel_config_t* config = NULL;
    ap3_adc_internal_channel_e desired_internal_channel = (instance) ? AP3_ADC_INTERNAL_CHANNELS_DIFF1 : AP3_ADC_INTERNAL_CHANNELS_DIFF0;
    for(size_t idx = 0; idx < (sizeof(ap3_adc_channel_configs)/sizeof(ap3_adc_channel_config_t)); idx++){
        if(ap3_adc_channel_configs[idx].internal_channel == desired_internal_channel){
            config = (ap3_adc_channel_configs + idx);
            break;
        }
    }
    if(!config){ return 0; }
    return ap3_analog_read(config);
}

int analogReadTemp( void ){
    ap3_adc_channel_config_t* config = NULL;
    for(size_t idx = 0; idx < (sizeof(ap3_adc_channel_configs)/sizeof(ap3_adc_channel_config_t)); idx++){
        if(ap3_adc_channel_configs[idx].internal_channel == AP3_ADC_INTERNAL_CHANNELS_TEMP){
            config = (ap3_adc_channel_configs + idx);
            break;
        }
    }
    if(!config){ return 0; }
    return ap3_analog_read(config);
}

int analogReadVCCDiv3( void ){
    ap3_adc_channel_config_t* config = NULL;
    for(size_t idx = 0; idx < (sizeof(ap3_adc_channel_configs)/sizeof(ap3_adc_channel_config_t)); idx++){
        if(ap3_adc_channel_configs[idx].internal_channel == AP3_ADC_INTERNAL_CHANNELS_VCC_DIV3){
            config = (ap3_adc_channel_configs + idx);
            break;
        }
    }
    if(!config){ return 0; }
    return ap3_analog_read(config);
}

int analogReadVSS( void ){
    ap3_adc_channel_config_t* config = NULL;
    for(size_t idx = 0; idx < (sizeof(ap3_adc_channel_configs)/sizeof(ap3_adc_channel_config_t)); idx++){
        if(ap3_adc_channel_configs[idx].internal_channel == AP3_ADC_INTERNAL_CHANNELS_VSS){
            config = (ap3_adc_channel_configs + idx);
            break;
        }
    }
    if(!config){ return 0; }
    return ap3_analog_read(config);
}

void indexAnalogWrite(pin_size_t index, int val){
    // todo: replace with mbed "PwmOut" functionality

    pin_size_t pinNumber = pinNameByIndex(index);
    if(pinNumber == (pin_size_t)NC){ return; }
    ap3_gpio_pad_t pad = pinNumber;

    uint32_t clk = AP3_ANALOG_CLK_SRC;
    uint32_t th = (uint32_t)((val * _analogWriteWidth) / ((0x01 << _analogWriteResolution) - 1));

    ap3_pwm_output(pad, th, _analogWriteWidth, clk);
}

void indexTone(pin_size_t index, unsigned int frequency, unsigned long duration){
    // todo: replace with mbed "PwmOut" functionality

    pin_size_t pinNumber = pinNumberByIndex(index);
    if(pinNumber == (pin_size_t)NC){ return; }
    ap3_gpio_pad_t pad = pinNumber;

    uint32_t clk = AP3_ANALOG_CLK_SRC;
    uint32_t fw = 0;

    if(!frequency){
        ap3_pwm_output(pad, 0, 0, clk);
        return;
    }

    if (frequency > 0){
        fw = AP3_ANALOG_CLK_FREQ / frequency;
    }
    uint32_t th = fw / 2; // 50% time high
    ap3_pwm_output(pad, th, fw, clk);

    if(duration){
        uint32_t stop_time = millis() + duration;
        while(millis() < stop_time){};
    }

    ap3_pwm_output(pad, 0, 0, clk);
}

ap3_err_t analogWriteResolution(uint8_t bits){
    if (bits > AP3_ANALOG_WRITE_RESOLUTION_MAX){
        _analogWriteResolution = AP3_ANALOG_WRITE_RESOLUTION_MAX;
        return AP3_ERR;
    }
    if (bits < AP3_ANALOG_WRITE_RESOLUTION_MIN){
        _analogWriteResolution = AP3_ANALOG_WRITE_RESOLUTION_MIN;
        return AP3_ERR;
    }
    _analogWriteResolution = bits;
    return AP3_OK;
}

ap3_err_t analogWriteFrameWidth(uint32_t width){
    if (width > AP3_MAX_ANALOG_WRITE_WIDTH){
        _analogWriteWidth = AP3_MAX_ANALOG_WRITE_WIDTH;
        return AP3_ERR;
    }
    _analogWriteWidth = width;
    return AP3_OK;
}

ap3_err_t analogWriteFrequency(float freq){    
    uint32_t new_width = (uint32_t)(AP3_ANALOG_CLK_FREQ / freq);

    if (new_width > AP3_MAX_ANALOG_WRITE_WIDTH){
        return AP3_ERR;
    }
    if (new_width < AP3_MIN_ANALOG_WRITE_WIDTH){
        return AP3_ERR;
    }
    _analogWriteWidth = new_width;
    return AP3_OK;
}

ap3_err_t servoWriteResolution(uint8_t bits){
    if (bits > AP3_ANALOG_WRITE_RESOLUTION_MAX){
        _servoWriteResolution = AP3_ANALOG_WRITE_RESOLUTION_MAX;
        return AP3_ERR;
    }
    if (bits < AP3_ANALOG_WRITE_RESOLUTION_MIN){
        _servoWriteResolution = AP3_ANALOG_WRITE_RESOLUTION_MIN;
        return AP3_ERR;
    }
    _servoWriteResolution = bits;
    return AP3_OK;
}

uint8_t getServoResolution(){
    return _servoWriteResolution;
}

ap3_err_t servoWrite(uint8_t pin, uint32_t val){
    return (servoWrite(pin, val, 544, 2400)); //Call servoWrite with Arduino default min/max microseconds. See: https://www.arduino.cc/en/Reference/ServoAttach
}

ap3_err_t servoWrite(uint8_t pin, uint32_t val, uint16_t minMicros, uint16_t maxMicros){
    // Determine the high time based on input value and the current resolution setting
    uint32_t fsv = (0x01 << _servoWriteResolution); // full scale value for the current resolution setting
    val = val % fsv;                                // prevent excess
    uint32_t clk = AM_HAL_CTIMER_HFRC_3MHZ;         // Using 3 MHz to get fine-grained control up to 20 ms wide
    uint32_t fw = 60000;                            // 20 ms wide frame

    //Convert microSeconds to PWM counts.
    uint32_t min = minMicros * 3;
    uint32_t max = maxMicros * 3;

    uint32_t th = (uint32_t)(((max - min) * val) / fsv) + min;

    return ap3_pwm_output(pin, th, fw, clk);
}

ap3_err_t analogReadResolution(uint8_t bits){
    if(bits > AP3_ANALOG_READ_RESOLUTION_MAX){
        _analogReadResolution = AP3_ANALOG_READ_RESOLUTION_MAX;
        return AP3_ERR;
    }
    if(bits < AP3_ANALOG_READ_RESOLUTION_MIN){
        _analogReadResolution = AP3_ANALOG_READ_RESOLUTION_MIN;
        return AP3_ERR;
    }
    _analogReadResolution = bits;
    return AP3_OK;
}

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

uint32_t powerControlADC(bool on){
    uint32_t status = AM_HAL_STATUS_SUCCESS;

    if(on){
        status = am_hal_adc_initialize(0, &g_ADCHandle);
        if(status != AM_HAL_STATUS_SUCCESS){ return status; }

        status = am_hal_adc_power_control(g_ADCHandle, AM_HAL_SYSCTRL_WAKE, false);
        if(status != AM_HAL_STATUS_SUCCESS){ return status; }

        adc_initialized = true;
    }else{
        adc_initialized = false;

        status = am_hal_adc_disable(g_ADCHandle);
        if(status != AM_HAL_STATUS_SUCCESS){ return status; }

        status = am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_ADC);
        if(status != AM_HAL_STATUS_SUCCESS){ return status; }

        status = am_hal_adc_deinitialize(g_ADCHandle);
        if(status != AM_HAL_STATUS_SUCCESS){ return status; }
    }

    return status;
}

uint32_t initializeADC( void ){
    am_hal_adc_config_t ADCConfig;

    // Power on the ADC.
    powerControlADC(true);

    // Set up the ADC configuration parameters. These settings are reasonable
    // for accurate measurements at a low sample rate.
    ADCConfig.eClock = AM_HAL_ADC_CLKSEL_HFRC;
    ADCConfig.ePolarity = AM_HAL_ADC_TRIGPOL_RISING;
    ADCConfig.eTrigger = AM_HAL_ADC_TRIGSEL_SOFTWARE;
    ADCConfig.eReference = AM_HAL_ADC_REFSEL_INT_2P0;
    ADCConfig.eClockMode = AM_HAL_ADC_CLKMODE_LOW_LATENCY;
    ADCConfig.ePowerMode = AM_HAL_ADC_LPMODE0;
    ADCConfig.eRepeat = AM_HAL_ADC_SINGLE_SCAN;

    return am_hal_adc_configure(g_ADCHandle, &ADCConfig);
}

ap3_err_t ap3_config_channel(ap3_adc_channel_config_t* config){
    am_hal_adc_slot_config_t ADCSlotConfig;

    // Set up an ADC slot
    ADCSlotConfig.eMeasToAvg = AM_HAL_ADC_SLOT_AVG_1;
    ADCSlotConfig.ePrecisionMode = AM_HAL_ADC_SLOT_14BIT;
    ADCSlotConfig.eChannel = config->eChannel;
    ADCSlotConfig.bWindowCompare = false;
    ADCSlotConfig.bEnabled = true;

    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_disable(g_ADCHandle)){
        return AP3_ERR;
    }

    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_configure_slot(g_ADCHandle, 0, &ADCSlotConfig)){
        return AP3_ERR;
    }

    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_enable(g_ADCHandle)){
        return AP3_ERR;
    }

    return AP3_OK;
}

int ap3_analog_read(ap3_adc_channel_config_t* config){
    if (!adc_initialized){
        initializeADC();
    }

    uint32_t ui32IntMask;
    am_hal_adc_sample_t Sample;
    uint32_t ui32NumSamples = 1;

    // configure the pin as an input if not an internal channel 
    // yes, configure on every analogRead - this is not much overhead in Arduino
    if(config->internal_channel == AP3_ADC_INTERNAL_CHANNELS_NUM){
        uint8_t funcsel = config->funcsel;
        am_hal_gpio_pincfg_t pincfg = g_AM_HAL_GPIO_INPUT;
        pincfg.uFuncSel = funcsel;
        am_hal_gpio_pinconfig(config->pinNumber, pincfg);
    }

    ap3_config_channel(config);

    // Clear the ADC interrupt.
    am_hal_adc_interrupt_status(g_ADCHandle, &ui32IntMask, false);
    MBED_ASSERT(AM_HAL_STATUS_SUCCESS == am_hal_adc_interrupt_clear(g_ADCHandle, ui32IntMask));

    am_hal_adc_sw_trigger(g_ADCHandle);

    do { // Wait for interrupt
        MBED_ASSERT(AM_HAL_STATUS_SUCCESS == am_hal_adc_interrupt_status(g_ADCHandle, &ui32IntMask, false));
    } while(!(ui32IntMask & AM_HAL_ADC_INT_CNVCMP));

    MBED_ASSERT(AM_HAL_STATUS_SUCCESS == am_hal_adc_samples_read(g_ADCHandle, false, NULL, &ui32NumSamples, &Sample));

    uint32_t result = Sample.ui32Sample;

    // Shift result depending on resolution
    return (_analogReadResolution > AP3_ADC_RESOLUTION) ? (result << (_analogReadResolution - AP3_ADC_RESOLUTION)) : (result >> (AP3_ADC_RESOLUTION - _analogReadResolution));
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

ap3_err_t ap3_pwm_output(ap3_gpio_pad_t pad, uint32_t th, uint32_t fw, uint32_t clk)
{
    if (fw > 0)
    { // reduce fw so that the user's desired value is the period
        fw--;
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
    bool set_periods = true;
    if ((th == 0) || (fw == 0))
    {
        output = AM_HAL_CTIMER_OUTPUT_FORCE0;
        set_periods = false; // disable setting periods when going into a forced mode
    }
    else if (th == fw)
    {
        output = AM_HAL_CTIMER_OUTPUT_FORCE1;
        set_periods = false; // disable setting periods when going into a forced mode
    }

    // Configure the pin
    am_hal_ctimer_output_config(timer,
                                segment,
                                pad,
                                output,
                                AM_HAL_GPIO_PIN_DRIVESTRENGTH_12MA); //

    // if timer is running wait for timer value to roll over (will indicate that at least one pulse has been emitted)
    AM_CRITICAL_BEGIN // critical section when reading / writing config registers
    if ((segment == AM_HAL_CTIMER_TIMERA && *((uint32_t *)CTIMERADDRn(CTIMER, timer, CTRL0)) & (CTIMER_CTRL0_TMRA0EN_Msk)) ||
        (segment == AM_HAL_CTIMER_TIMERB && *((uint32_t *)CTIMERADDRn(CTIMER, timer, CTRL0)) & (CTIMER_CTRL0_TMRB0EN_Msk)))
    {
        uint32_t current = 0;
        uint32_t last = 0;
        do
        {
            last = current;
            current = am_hal_ctimer_read(timer, segment);
        } while (current >= last);
    }
    AM_CRITICAL_END // end critical section

    // clear timer (also stops the timer)
    am_hal_ctimer_clear(timer, segment);

    // Configure the repeated pulse mode with our clock source
    am_hal_ctimer_config_single(timer,
                                segment,
                                // (AM_HAL_CTIMER_FN_PWM_REPEAT | AP3_ANALOG_CLK_SRC | AM_HAL_CTIMER_INT_ENABLE) );
                                (AM_HAL_CTIMER_FN_PWM_REPEAT | clk));

    if (set_periods)
    {
        // If this pad uses secondary output:
        if (output == AM_HAL_CTIMER_OUTPUT_SECONDARY)
        {
            // Need to explicitly enable compare registers 2/3
            uint32_t *pui32ConfigReg = NULL;
            pui32ConfigReg = (uint32_t *)CTIMERADDRn(CTIMER, timer, AUX0);
            uint32_t ui32WriteVal = AM_REGVAL(pui32ConfigReg);
            uint32_t ui32ConfigVal = (1 << CTIMER_AUX0_TMRA0EN23_Pos); // using CTIMER_AUX0_TMRA0EN23_Pos because for now this number is common to all CTimer instances
            volatile uint32_t *pui32CompareRegA = (uint32_t *)CTIMERADDRn(CTIMER, timer, CMPRA0);
            volatile uint32_t *pui32CompareRegB = (uint32_t *)CTIMERADDRn(CTIMER, timer, CMPRB0);
            uint32_t masterPeriod = (uint32_t)(*(pui32CompareRegA)&CTIMER_CMPRA0_CMPR1A0_Msk) >> CTIMER_CMPRA0_CMPR1A0_Pos;
            uint32_t masterRisingTrigger = (uint32_t)(*(pui32CompareRegA)&CTIMER_CMPRA0_CMPR0A0_Msk) >> CTIMER_CMPRA0_CMPR0A0_Pos;

            if (segment == AM_HAL_CTIMER_TIMERB)
            {
                ui32ConfigVal = ((ui32ConfigVal & 0xFFFF) << 16);
                masterPeriod = (uint32_t)(*(pui32CompareRegB)&CTIMER_CMPRB0_CMPR1B0_Msk) >> CTIMER_CMPRB0_CMPR1B0_Pos;
                masterRisingTrigger = (uint32_t)(*(pui32CompareRegA)&CTIMER_CMPRB0_CMPR0B0_Msk) >> CTIMER_CMPRB0_CMPR0B0_Pos;
            }
            ui32WriteVal |= ui32ConfigVal;
            AM_REGVAL(pui32ConfigReg) = ui32WriteVal;

            if (masterPeriod != fw)
            {
                // the master output fw dictates the secondary fw... so if they are different try to change the master while preserving duty cycle
                uint32_t masterTH = ((masterPeriod - masterRisingTrigger) * fw) / masterPeriod; // try to compensate in case _analogWriteWidth was changed
                if (masterPeriod == 0)
                { // if masterPeriod was 0 then masterTH will be invalid (divide by 0). This usually means that the master timer output did not have a set duty cycle. This also means the output is probably not configured and so it is okay to choose an arbitrary duty cycle
                    masterTH = fw - 1;
                }
                am_hal_ctimer_period_set(timer, segment, fw, masterTH); // but this overwrites the non-aux compare regs for this timer / segment
                // Serial.printf("th = %d, fw = %d, (masterPeriod - masterRisingTrigger) = (%d - %d) = %d\n", th, fw, masterPeriod, masterRisingTrigger, (masterPeriod - masterRisingTrigger));
            }

            // then set the duty cycle with the 'aux' function
            am_hal_ctimer_aux_period_set(timer, segment, fw, th);
        }
        else
        {
            // Try to preserve settings of the secondary output
            uint32_t *pui32ConfigReg = NULL;
            pui32ConfigReg = (uint32_t *)CTIMERADDRn(CTIMER, timer, AUX0);
            volatile uint32_t *pui32CompareRegA = (uint32_t *)CTIMERADDRn(CTIMER, timer, CMPRAUXA0);
            volatile uint32_t *pui32CompareRegB = (uint32_t *)CTIMERADDRn(CTIMER, timer, CMPRAUXB0);
            uint32_t slavePeriod = (uint32_t)(*(pui32CompareRegA)&CTIMER_CMPRA0_CMPR1A0_Msk) >> CTIMER_CMPRA0_CMPR1A0_Pos;
            uint32_t slaveRisingTrigger = (uint32_t)(*(pui32CompareRegA)&CTIMER_CMPRA0_CMPR0A0_Msk) >> CTIMER_CMPRA0_CMPR0A0_Pos;

            uint32_t auxEnabled = (AM_REGVAL(pui32ConfigReg) & CTIMER_AUX0_TMRA0EN23_Msk);

            if (segment == AM_HAL_CTIMER_TIMERB)
            {
                auxEnabled = (AM_REGVAL(pui32ConfigReg) & (CTIMER_AUX0_TMRA0EN23_Msk << 16));
                slavePeriod = (uint32_t)(*(pui32CompareRegB)&CTIMER_CMPRB0_CMPR1B0_Msk) >> CTIMER_CMPRB0_CMPR1B0_Pos;
                slaveRisingTrigger = (uint32_t)(*(pui32CompareRegA)&CTIMER_CMPRB0_CMPR0B0_Msk) >> CTIMER_CMPRB0_CMPR0B0_Pos;
            }

            if (auxEnabled)
            { // if secondary outputs are enabled
                if (slavePeriod != fw)
                {                                                                               // and if fw is different from previous slavePeriod
                    uint32_t slaveTH = ((slavePeriod - slaveRisingTrigger) * fw) / slavePeriod; // try to compensate in case _analogWriteWidth was changed
                    if (slavePeriod == 0)
                    { // if masterPeriod was 0 then masterTH will be invalid (divide by 0). This usually means that the master timer output did not have a set duty cycle. This also means the output is probably not configured and so it is okay to choose an arbitrary duty cycle
                        slaveTH = fw - 1;
                    }
                    am_hal_ctimer_aux_period_set(timer, segment, fw, slaveTH); // but this overwrites the non-aux compare regs for this timer / segment
                }
            }

            // Now set the primary duty cycle
            am_hal_ctimer_period_set(timer, segment, fw, th);
        }

        am_hal_ctimer_start(timer, segment); // Start the timer only when there are periods to compare to
    }

    return AP3_OK;
}
