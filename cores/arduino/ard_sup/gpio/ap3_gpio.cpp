#include "ap3_gpio.h"
#include "variant.h"

typedef struct{
    voidFuncPtrArgs callback;   // the callback function
    void*           arg;        // argument
    uint8_t         mode;       // Arduino interrupt mode
    uint8_t         pad;        // which apollo3 pad 
}ap3_gpio_isr_entry_t;
ap3_gpio_isr_entry_t    gpio_isr_entries[AP3_GPIO_MAX_PADS] = {NULL};
uint8_t                 gpio_num_isr = 0;




ap3_gpio_pad_t ap3_gpio_pin2pad(ap3_gpio_pin_t pin)
{
    return ap3_variant_pinmap[pin];
}

typedef void (*voidFuncPtr)(void);
typedef void (*voidFuncPtrArg)(void *);
typedef struct
{
    voidFuncPtr fn;
    void *arg;
    bool functional;
} InterruptHandle_t;
// static InterruptHandle_t __pinInterruptHandlers[AP3_GPIO_MAX_EXT_INT_HANDLERS] = {0,};

void padMode(uint8_t pad, am_hal_gpio_pincfg_t mode, ap3_err_t *retval)
{
    if (!ap3_gpio_is_valid(pad))
    {
        if (retval != NULL)
        {
            *retval = AP3_OUT_OF_RANGE;
        }
        return;
    }
    am_hal_gpio_pinconfig(pad, mode);

    //Reset analog configuration flag if this pin has analog capabilities
    uint8_t indi;
    for (indi = 0; indi < AP3_ANALOG_PADS; indi++)
    {
        if (ap3_analog_configure_map[indi].pad == pad)
        {
            ap3_analog_configure_map[indi].isAnalog = false;
            break;
        }
    }
    if (indi == AP3_ANALOG_PADS)
    {
        //Serial.println("Error - analog status reset, pin not found");
    }
}

void padMode(uint8_t pad, am_hal_gpio_pincfg_t mode)
{
    padMode(pad, mode, NULL);
}

void pinMode(uint8_t pin, am_hal_gpio_pincfg_t mode, ap3_err_t *retval)
{
    ap3_gpio_pad_t pad = ap3_gpio_pin2pad(pin);
    padMode(pad, mode, retval);
}

void pinMode(uint8_t pin, am_hal_gpio_pincfg_t mode)
{
    pinMode(pin, mode, NULL);
}

extern void digitalWrite(uint8_t pin, uint8_t val)
{
    ap3_gpio_pad_t pad = ap3_gpio_pin2pad(pin);
    if (!ap3_gpio_is_valid(pad))
    {
        return;
    }
    if (val)
    {
        am_hal_gpio_output_set(ap3_gpio_pin2pad(pin));
    }
    else
    {
        am_hal_gpio_output_clear(ap3_gpio_pin2pad(pin));
    }
}

extern int digitalRead(uint8_t pin)
{
    ap3_gpio_pad_t pad = ap3_gpio_pin2pad(pin);
    if (!ap3_gpio_is_valid(pad))
    {
        return 0;
    }
    // determine if output or input
    uint32_t ui32BaseAddr = pad / 8;
    uint32_t ui32BaseShift = ((pad % 8) * 4) + 1;
    uint8_t output = ((AM_REGVAL(&GPIO->CFGA + ui32BaseAddr) >> ui32BaseShift) & 0x03);

    if (output)
    {
        return (int)am_hal_gpio_output_read(pad);
    }
    else
    {
        return (int)am_hal_gpio_input_read(pad);
    }
    return 0;
}

// static intr_handle_t gpio_intr_handle = NULL;

extern "C" void am_gpio_isr(void)
{
    uint64_t gpio_int_mask = 0x00;
    am_hal_gpio_interrupt_status_get(true, &gpio_int_mask);

    for(uint8_t indi = 0; indi < gpio_num_isr; indi++){
        if( gpio_isr_entries[indi].callback != NULL ){
            if( gpio_int_mask & AM_HAL_GPIO_BIT(gpio_isr_entries[indi].pad) ){
                gpio_isr_entries[indi].callback(gpio_isr_entries[indi].arg);
            }
            if( !((gpio_isr_entries[indi].mode == LOW) || (gpio_isr_entries[indi].mode == HIGH)) ){    // if not a HIGH or LOW interrupt then clear the flag
                am_hal_gpio_interrupt_clear(AM_HAL_GPIO_BIT(gpio_isr_entries[indi].pad));
            }
        }
    }
}

void attachInterruptArg(uint8_t pin, voidFuncPtrArgs callbackArgs, void * arg, int mode)
{
    ap3_gpio_pad_t pad = ap3_gpio_pin2pad(pin);
    if( pad == AP3_GPIO_PAD_UNUSED ){ return; }

    if( gpio_num_isr < AP3_GPIO_MAX_PADS ){
        uint8_t indi = 0;
        for(indi = 0; indi < gpio_num_isr; indi++){
            if(gpio_isr_entries[indi].pad == pad){
                break;
            }
        }

        gpio_isr_entries[indi].pad = pad;
        gpio_isr_entries[indi].callback = callbackArgs;
        gpio_isr_entries[indi].arg = arg;
        gpio_isr_entries[indi].mode = mode;

        // todo: need to somehow enable interrupts for the pad in question (Arduino does this by default when attachInterrupt is called)
        // needs to respect the 'mode'
        // needs to no overwrite the old configuration
        am_hal_gpio_pincfg_t int_pincfg; 
        if(( mode == FALLING ) || ( mode == LOW )){
            int_pincfg.eIntDir = AM_HAL_GPIO_PIN_INTDIR_HI2LO;
        }else if(( mode == RISING ) || ( mode == HIGH )){
            int_pincfg.eIntDir = AM_HAL_GPIO_PIN_INTDIR_LO2HI;
        }else{
            int_pincfg.eIntDir = AM_HAL_GPIO_PIN_INTDIR_BOTH; 
        }
        ap3_gpio_pinconfig_ORnot( pad, int_pincfg, true);

        // clear the flag and enable the interrupt in the NVIC 
        am_hal_gpio_interrupt_clear(AM_HAL_GPIO_BIT(pad));
        am_hal_gpio_interrupt_enable(AM_HAL_GPIO_BIT(pad));
        NVIC_EnableIRQ(GPIO_IRQn);

        if( indi == gpio_num_isr ){
            gpio_num_isr++;
        }
    }
}

void attachInterrupt(uint8_t pin, voidFuncPtr callback, int mode)
{
    attachInterruptArg( pin, (voidFuncPtrArgs)callback, NULL, mode );
}

extern void detachInterrupt(uint8_t pin)
{
    ap3_gpio_pad_t pad = ap3_gpio_pin2pad(pin);
    if( pad == AP3_GPIO_PAD_UNUSED ){ return; }

    uint8_t indi = 0;

    // Look for an entry that matches the pad
    for(indi = 0; indi < gpio_num_isr; indi++){
        if(gpio_isr_entries[indi].pad == pad){
            break;
        }
    }

    // return if it was not found (prevent changes to the isr entry list)
    if( indi == gpio_num_isr ){
        return;
    }

    // disable interrupts for the given pad without blasting the configuration
    am_hal_gpio_pincfg_t int_pincfg; 
    int_pincfg.eIntDir = AM_HAL_GPIO_PIN_INTDIR_BOTH; // disable any interrupts
    
    ap3_gpio_pinconfig_ORnot( pad, int_pincfg, false);

    // disable the interrupt in the NVIC
    am_hal_gpio_interrupt_disable(AM_HAL_GPIO_BIT(pad));

    // Shift down the remaining interrupt entries
    for(indi; indi < gpio_num_isr-1; indi++){
        gpio_isr_entries[indi] = gpio_isr_entries[indi+1];
    }
    
    // Clear out the last entry
    gpio_isr_entries[gpio_num_isr].pad = 0;
    gpio_isr_entries[gpio_num_isr].callback = NULL;
    gpio_isr_entries[gpio_num_isr].mode = LOW;
    gpio_isr_entries[gpio_num_isr].arg = NULL;

}


















uint32_t ap3_gpio_pinconfig_ORnot(uint32_t ui32Pin, am_hal_gpio_pincfg_t bfGpioCfg, bool ORnot)
{
//     uint32_t ui32Padreg, ui32AltPadCfg, ui32GPCfg;
//     uint32_t ui32Funcsel, ui32PowerSw;
//     bool bClearEnable = false;

// #ifndef AM_HAL_DISABLE_API_VALIDATION
//     if (ui32Pin >= AM_HAL_GPIO_MAX_PADS)
//     {
//         return AM_HAL_STATUS_INVALID_ARG;
//     }
// #endif // AM_HAL_DISABLE_API_VALIDATION

//     //
//     // Initialize the PADREG accumulator variables.
//     //
//     ui32GPCfg = ui32Padreg = ui32AltPadCfg = 0;

//     //
//     // Get the requested function and/or power switch.
//     //
//     ui32Funcsel = bfGpioCfg.uFuncSel;
//     ui32PowerSw = bfGpioCfg.ePowerSw;

//     ui32Padreg |= ui32Funcsel << PADREG_FLD_FNSEL_S;

//     //
//     // Check for invalid configuration requests.
//     //
//     if (bfGpioCfg.ePullup != AM_HAL_GPIO_PIN_PULLUP_NONE)
//     {
//         //
//         // This setting is needed for all pullup settings including
//         // AM_HAL_GPIO_PIN_PULLUP_WEAK and AM_HAL_GPIO_PIN_PULLDOWN.
//         //
//         ui32Padreg |= (0x1 << PADREG_FLD_PULLUP_S);

//         //
//         // Check for specific pullup or pulldown settings.
//         //
//         if ((bfGpioCfg.ePullup >= AM_HAL_GPIO_PIN_PULLUP_1_5K) &&
//             (bfGpioCfg.ePullup <= AM_HAL_GPIO_PIN_PULLUP_24K))
//         {
//             ui32Padreg |= ((bfGpioCfg.ePullup - AM_HAL_GPIO_PIN_PULLUP_1_5K) << PADREG_FLD_76_S);
// #ifndef AM_HAL_DISABLE_API_VALIDATION
//             if (!(g_ui8Bit76Capabilities[ui32Pin] & CAP_PUP))
//             {
//                 return AM_HAL_GPIO_ERR_PULLUP;
//             }
//         }
//         else if (bfGpioCfg.ePullup == AM_HAL_GPIO_PIN_PULLDOWN)
//         {
//             if (ui32Pin != 20)
//             {
//                 return AM_HAL_GPIO_ERR_PULLDOWN;
//             }
//         }
//         else if (bfGpioCfg.ePullup != AM_HAL_GPIO_PIN_PULLUP_WEAK)
//         {
//             if ((g_ui8Bit76Capabilities[ui32Pin] & (CAP_PUP | CAP_PDN)) == 0)
//             {
//                 return AM_HAL_GPIO_ERR_PULLUP;
//             }
// #endif // AM_HAL_DISABLE_API_VALIDATION
//         }
//     }

//     //
//     // Check if requesting a power switch pin
//     //
//     if (ui32PowerSw != AM_HAL_GPIO_PIN_POWERSW_NONE)
//     {
//         if ((ui32PowerSw == AM_HAL_GPIO_PIN_POWERSW_VDD) &&
//             (g_ui8Bit76Capabilities[ui32Pin] & CAP_VDD))
//         {
//             ui32Padreg |= 0x1 << PADREG_FLD_76_S;
//         }
//         else if ((ui32PowerSw == AM_HAL_GPIO_PIN_POWERSW_VSS) &&
//                  (g_ui8Bit76Capabilities[ui32Pin] & CAP_VSS))
//         {
//             ui32Padreg |= 0x2 << PADREG_FLD_76_S;
//         }
//         else
//         {
//             return AM_HAL_GPIO_ERR_PWRSW;
//         }
//     }

//     //
//     // Depending on the selected pin and FNSEL, determine if INPEN needs to be set.
//     //
//     ui32Padreg |= (g_ui8Inpen[ui32Pin] & (1 << ui32Funcsel)) ? (1 << PADREG_FLD_INPEN_S) : 0;

//     //
//     // Configure ui32GpCfg based on whether nCE requested.
//     //
//     if (g_ui8nCEpins[ui32Pin] == ui32Funcsel)
//     {
//         uint32_t ui32Outcfg;
//         uint8_t ui8CEtbl;

// #ifndef AM_HAL_DISABLE_API_VALIDATION
//         //
//         // User is configuring a nCE. Verify the requested settings and set the
//         // polarity and OUTCFG values (INCFG is not used here and should be 0).
//         // Valid uNCE values are 0-3 (uNCE is a 2-bit field).
//         // Valid uIOMnum are 0-6 (0-5 for IOMs, 6 for MSPI, 7 is invalid).
//         //
//         if (bfGpioCfg.uIOMnum > IOMNUM_MAX)
//         {
//             return AM_HAL_GPIO_ERR_INVCE; // Invalid CE specified
//         }
// #endif // AM_HAL_DISABLE_API_VALIDATION

//         //
//         // Construct the entry we expect to find in the table. We can determine
//         // the OUTCFG value by looking for that value in the pin row.
//         //
//         ui8CEtbl = (bfGpioCfg.uIOMnum << 4) | bfGpioCfg.uNCE;
//         for (ui32Outcfg = 0; ui32Outcfg < 4; ui32Outcfg++)
//         {
//             if (g_ui8NCEtable[ui32Pin][ui32Outcfg] == ui8CEtbl)
//             {
//                 break;
//             }
//         }

// #ifndef AM_HAL_DISABLE_API_VALIDATION
//         if (ui32Outcfg >= 4)
//         {
//             return AM_HAL_GPIO_ERR_INVCEPIN;
//         }
// #endif // AM_HAL_DISABLE_API_VALIDATION

//         ui32GPCfg |= (ui32Outcfg << GPIOCFG_FLD_OUTCFG_S) |
//                      (bfGpioCfg.eCEpol << GPIOCFG_FLD_INTD_S) |
//                      (0 << GPIOCFG_FLD_INCFG_S);
//     }
//     else
//     {
//         //
//         // It's not nCE, it's one of the other funcsels.
//         // Start by setting the value of the requested GPIO input.
//         //
//         ui32Padreg |= (bfGpioCfg.eGPInput << PADREG_FLD_INPEN_S);

//         //
//         // Map the requested interrupt direction settings into the Apollo3
//         //  GPIOCFG register field, which is a 4-bit field:
//         //  [INTD(1):OUTCFG(2):INCFG(1)].
//         // Bit0 of eIntDir maps to GPIOCFG.INTD  (b3).
//         // Bit1 of eIntDir maps to GPIOCFG.INCFG (b0).
//         //
//         ui32GPCfg |= (bfGpioCfg.eGPOutcfg << GPIOCFG_FLD_OUTCFG_S) |
//                      (((bfGpioCfg.eIntDir >> 0) & 0x1) << GPIOCFG_FLD_INTD_S) |
//                      (((bfGpioCfg.eIntDir >> 1) & 0x1) << GPIOCFG_FLD_INCFG_S);

//         if ((bfGpioCfg.eGPOutcfg == AM_HAL_GPIO_PIN_OUTCFG_PUSHPULL) ||
//             pincfg_equ(&bfGpioCfg, (void *)&g_AM_HAL_GPIO_DISABLE))
//         {
//             //
//             // For pushpull configurations, we must be sure to clear the ENABLE
//             // bit.  In pushpull, these bits turn on FAST GPIO.  For regular
//             // GPIO, they must be clear.
//             //
//             bClearEnable = true;
//         }

//         //
//         // There is some overlap between eGPRdZero and eIntDir as both settings
//         //  utilize the overloaded INCFG bit.
//         // Therefore the two fields should be used in a mutually exclusive
//         //  manner. For flexibility however they are not disallowed because
//         //  their functionality is dependent on FUNCSEL and whether interrupts
//         //  are used.
//         //
//         // In the vein of mutual exclusion, eGPRdZero is primarily intended for
//         //  use when GPIO interrupts are not in use and can be used when no
//         //  eIntDir setting is provided.
//         // If eIntDir is provided, eGPRdZero is ignored and can only be
//         //  achieved via the AM_HAL_GPIO_PIN_INTDIR_NONE setting.
//         //
//         if (bfGpioCfg.eIntDir == 0)
//         {
//             ui32GPCfg &= ~(1 << GPIOCFG_FLD_INCFG_S);
//             ui32GPCfg |= (bfGpioCfg.eGPRdZero << GPIOCFG_FLD_INCFG_S);
//         }
//     }

//     switch (bfGpioCfg.eDriveStrength)
//     {
//     // DRIVESTRENGTH is a 2-bit field.
//     //  bit0 maps to bit2 of a PADREG field.
//     //  bit1 maps to bit0 of an ALTPADCFG field.
//     case AM_HAL_GPIO_PIN_DRIVESTRENGTH_2MA:
//         ui32Padreg |= (0 << PADREG_FLD_DRVSTR_S);
//         ui32AltPadCfg |= (0 << 0);
//         break;
//     case AM_HAL_GPIO_PIN_DRIVESTRENGTH_4MA:
//         ui32Padreg |= (1 << PADREG_FLD_DRVSTR_S);
//         ui32AltPadCfg |= (0 << 0);
//         break;
//     case AM_HAL_GPIO_PIN_DRIVESTRENGTH_8MA:
//         ui32Padreg |= (0 << PADREG_FLD_DRVSTR_S);
//         ui32AltPadCfg |= (1 << 0);
//         break;
//     case AM_HAL_GPIO_PIN_DRIVESTRENGTH_12MA:
//         ui32Padreg |= (1 << PADREG_FLD_DRVSTR_S);
//         ui32AltPadCfg |= (1 << 0);
//         break;
//     }

//     //
//     // At this point, the 3 configuration variables, ui32GpioCfg, ui32Padreg,
//     //  and ui32AltPadCfg values are set (at bit position 0) and ready to write
//     //  to their respective register bitfields.
//     //
//     uint32_t ui32GPCfgAddr, ui32PadregAddr, ui32AltpadAddr;
//     uint32_t ui32GPCfgClearMask, ui32PadClearMask;
//     uint32_t ui32GPCfgShft, ui32PadShft;

//     ui32GPCfgAddr = AM_REGADDR(GPIO, CFGA) + ((ui32Pin >> 1) & ~0x3);
//     ui32PadregAddr = AM_REGADDR(GPIO, PADREGA) + (ui32Pin & ~0x3);
//     ui32AltpadAddr = AM_REGADDR(GPIO, ALTPADCFGA) + (ui32Pin & ~0x3);

//     ui32GPCfgShft = ((ui32Pin & 0x7) << 2);
//     ui32PadShft = ((ui32Pin & 0x3) << 3);
//     ui32GPCfgClearMask = ~((uint32_t)0xF << ui32GPCfgShft);
//     ui32PadClearMask = ~((uint32_t)0xFF << ui32PadShft);

//     //
//     // Get the new values into their rightful bit positions.
//     //
//     ui32Padreg <<= ui32PadShft;
//     ui32AltPadCfg <<= ui32PadShft;
//     ui32GPCfg <<= ui32GPCfgShft;

//     AM_CRITICAL_BEGIN

//     if (bClearEnable)
//     {
//         //
//         // We're configuring a mode that requires clearing the Enable bit.
//         //
//         am_hal_gpio_output_tristate_disable(ui32Pin);
//     }

//     GPIO->PADKEY = GPIO_PADKEY_PADKEY_Key;

//     // Here's where the magic happens
//     if(ORnot){
//         AM_REGVAL(ui32PadregAddr) = (AM_REGVAL(ui32PadregAddr) & ui32PadClearMask) | ui32Padreg;
//         AM_REGVAL(ui32GPCfgAddr) = (AM_REGVAL(ui32GPCfgAddr) & ui32GPCfgClearMask) | ui32GPCfg;
//         AM_REGVAL(ui32AltpadAddr) = (AM_REGVAL(ui32AltpadAddr) & ui32PadClearMask) | ui32AltPadCfg;
//     }else{
//         AM_REGVAL(ui32PadregAddr) = (AM_REGVAL(ui32PadregAddr) & ui32PadClearMask) & ~ui32Padreg;
//         AM_REGVAL(ui32GPCfgAddr) = (AM_REGVAL(ui32GPCfgAddr) & ui32GPCfgClearMask) & ~ui32GPCfg;
//         AM_REGVAL(ui32AltpadAddr) = (AM_REGVAL(ui32AltpadAddr) & ui32PadClearMask) & ~ui32AltPadCfg;
//     }


//     GPIO->PADKEY = 0;

//     AM_CRITICAL_END

    return AM_HAL_STATUS_SUCCESS;

} // am_hal_gpio_pinconfig()