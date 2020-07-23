#include "ap3_gpio.h"
#include "variant.h"

typedef struct
{
    voidFuncPtrArgs callback; // the callback function
    void *arg;                // argument
    uint8_t mode;             // Arduino interrupt mode
    uint8_t pad;              // which apollo3 pad
} ap3_gpio_isr_entry_t;
ap3_gpio_isr_entry_t gpio_isr_entries[AP3_GPIO_MAX_PADS] = {NULL};
uint8_t gpio_num_isr = 0;

//*****************************************************************************
//  Local function declarations
//*****************************************************************************
static inline uint32_t ap3_get_funct_sel(ap3_gpio_pad_t pad);

//*****************************************************************************
//  Local defines.
//*****************************************************************************
//
// Generally define GPIO PADREG and GPIOCFG bitfields
// Copied from am_hal_gpio.c
#define PADREG_FLD_76_S 6
#define PADREG_FLD_FNSEL_S 3
#define PADREG_FLD_DRVSTR_S 2
#define PADREG_FLD_INPEN_S 1
#define PADREG_FLD_PULLUP_S 0

#define GPIOCFG_FLD_INTD_S 3
#define GPIOCFG_FLD_OUTCFG_S 1
#define GPIOCFG_FLD_INCFG_S 0

//Additional Defines
#define PADREG_FNSEL_Msk 0x38
#define GPIO_FUNCTION 3

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

// translate Arduino style pin mode function to apollo3 implementation
void pinMode(uint8_t pin, uint8_t mode)
{

    am_hal_gpio_pincfg_t pinmode = AP3_GPIO_PINCFG_NULL;

    switch (mode)
    {
    case INPUT:
        pinmode = AP3_PINCFG_INPUT;
        break;
    case OUTPUT:
        pinmode = AP3_PINCFG_OUTPUT;
        break;
    case INPUT_PULLUP:
        pinmode = AP3_PINCFG_INPUT_PULLUP;
        break;
    case INPUT_PULLDOWN:
        pinmode = AP3_PINCFG_INPUT_PULLDOWN;
        break;
    case OPEN_DRAIN:
        pinmode = AP3_PINCFG_OPEN_DRAIN;
        break;
    case TRISTATE:
        pinmode = AP3_PINCFG_TRISTATE;
        break;
    default:
        //no match, just do nothing
        return;
    }

    pinMode(pin, pinmode);
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

static inline uint32_t ap3_get_funct_sel(ap3_gpio_pad_t pad)
{
    uint32_t padregAddr = AM_REGADDR(GPIO, PADREGA) + (pad & ~0x3);
    uint32_t padShft = ((pad & 0x3) << 3);
    uint32_t functSelShift = PADREG_FLD_FNSEL_S;
    uint32_t functSelMask = PADREG_FNSEL_Msk;

    return (((AM_REGVAL(padregAddr) >> padShft) & functSelMask) >> functSelShift);
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

    for (uint8_t indi = 0; indi < gpio_num_isr; indi++)
    {
        if (gpio_isr_entries[indi].callback != NULL)
        {
            if (gpio_int_mask & AM_HAL_GPIO_BIT(gpio_isr_entries[indi].pad))
            {
                gpio_isr_entries[indi].callback(gpio_isr_entries[indi].arg);
            }
            if (!((gpio_isr_entries[indi].mode == LOW) || (gpio_isr_entries[indi].mode == HIGH)))
            { // if not a HIGH or LOW interrupt then clear the flag
                am_hal_gpio_interrupt_clear(AM_HAL_GPIO_BIT(gpio_isr_entries[indi].pad));
            }
            else
            { // In the case of a HIGH or LOW mode interrupt we need to manually check for the end state
                uint8_t val = digitalRead(gpio_isr_entries[indi].pad);
                if (gpio_isr_entries[indi].mode == LOW)
                {
                    if (val)
                    {
                        am_hal_gpio_interrupt_clear(AM_HAL_GPIO_BIT(gpio_isr_entries[indi].pad));
                    }
                }
                else
                {
                    if (!val)
                    {
                        am_hal_gpio_interrupt_clear(AM_HAL_GPIO_BIT(gpio_isr_entries[indi].pad));
                    }
                }
            }
        }
    }
}

void attachInterruptArg(uint8_t pin, voidFuncPtrArgs callbackArgs, void *arg, int mode)
{
    ap3_gpio_pad_t pad = ap3_gpio_pin2pad(pin);
    if (pad == AP3_GPIO_PAD_UNUSED)
    {
        return;
    }

    if (gpio_num_isr < AP3_GPIO_MAX_PADS)
    {
        uint8_t indi = 0;
        for (indi = 0; indi < gpio_num_isr; indi++)
        {
            if (gpio_isr_entries[indi].pad == pad)
            {
                break;
            }
        }

        gpio_isr_entries[indi].pad = pad;
        gpio_isr_entries[indi].callback = callbackArgs;
        gpio_isr_entries[indi].arg = arg;
        gpio_isr_entries[indi].mode = mode;

        // enable interrupts for the pad in question (Arduino does this by default when attachInterrupt is called)
        uint8_t eIntDir = 0x00;
        if ((mode == FALLING) || (mode == LOW))
        {
            eIntDir = AM_HAL_GPIO_PIN_INTDIR_HI2LO;
        }
        else if ((mode == RISING) || (mode == HIGH))
        {
            eIntDir = AM_HAL_GPIO_PIN_INTDIR_LO2HI;
        }
        else
        {
            eIntDir = AM_HAL_GPIO_PIN_INTDIR_BOTH;
        }
        ap3_gpio_enable_interrupts(pad, eIntDir);

        // clear the flag and enable the interrupt in the NVIC
        am_hal_gpio_interrupt_clear(AM_HAL_GPIO_BIT(pad));
        am_hal_gpio_interrupt_enable(AM_HAL_GPIO_BIT(pad));
        NVIC_EnableIRQ(GPIO_IRQn);

        if (indi == gpio_num_isr)
        {
            gpio_num_isr++;
        }
    }
}

void attachInterrupt(uint8_t pin, voidFuncPtr callback, int mode)
{
    attachInterruptArg(pin, (voidFuncPtrArgs)callback, NULL, mode);
}

extern void detachInterrupt(uint8_t pin)
{
    ap3_gpio_pad_t pad = ap3_gpio_pin2pad(pin);
    if (pad == AP3_GPIO_PAD_UNUSED)
    {
        return;
    }

    uint8_t indi = 0;

    // Look for an entry that matches the pad
    for (indi = 0; indi < gpio_num_isr; indi++)
    {
        if (gpio_isr_entries[indi].pad == pad)
        {
            break;
        }
    }

    // return if it was not found (prevent changes to the isr entry list)
    if (indi == gpio_num_isr)
    {
        return;
    }

    // disable the interrupt in the NVIC
    am_hal_gpio_interrupt_disable(AM_HAL_GPIO_BIT(pad));
    am_hal_gpio_interrupt_clear(AM_HAL_GPIO_BIT(pad));

    // disable interrupts for the given pad without blasting the configuration
    ap3_gpio_enable_interrupts(pad, AM_HAL_GPIO_PIN_INTDIR_NONE);

    // Shift down the remaining interrupt entries
    for (indi; indi < gpio_num_isr - 1; indi++)
    {
        gpio_isr_entries[indi] = gpio_isr_entries[indi + 1];
    }

    // Clear out the last entry
    gpio_isr_entries[gpio_num_isr].pad = 0;
    gpio_isr_entries[gpio_num_isr].callback = NULL;
    gpio_isr_entries[gpio_num_isr].mode = LOW;
    gpio_isr_entries[gpio_num_isr].arg = NULL;
    gpio_num_isr--;
}

uint32_t ap3_gpio_enable_interrupts(uint32_t ui32Pin, uint32_t eIntDir)
{
    uint32_t ui32Padreg, ui32AltPadCfg, ui32GPCfg;
    uint32_t ui32Funcsel, ui32PowerSw;
    bool bClearEnable = false;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    if (ui32Pin >= AM_HAL_GPIO_MAX_PADS)
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }
#endif // AM_HAL_DISABLE_API_VALIDATION

    //
    // Initialize the PADREG accumulator variables.
    //
    ui32GPCfg = ui32Padreg = ui32AltPadCfg = 0;

    //
    // Map the requested interrupt direction settings into the Apollo3
    //  GPIOCFG register field, which is a 4-bit field:
    //  [INTD(1):OUTCFG(2):INCFG(1)].
    // Bit0 of eIntDir maps to GPIOCFG.INTD  (b3).
    // Bit1 of eIntDir maps to GPIOCFG.INCFG (b0).
    //
    ui32GPCfg |= (((eIntDir >> 0) & 0x1) << GPIOCFG_FLD_INTD_S) | (((eIntDir >> 1) & 0x1) << GPIOCFG_FLD_INCFG_S);

    //
    // At this point, the configuration variable, ui32GpioCfg
    // value is set (at bit position 0) and ready to write
    //  to their respective register bitfields.
    //
    uint32_t ui32GPCfgAddr;
    uint32_t ui32GPCfgClearMask;
    uint32_t ui32GPCfgShft;

    ui32GPCfgShft = ((ui32Pin & 0x7) << 2);

    ui32GPCfgAddr = AM_REGADDR(GPIO, CFGA) + ((ui32Pin >> 1) & ~0x3);

    ui32GPCfgClearMask = ~((uint32_t)0xF << ui32GPCfgShft);

    //
    // Get the new values into their rightful bit positions.
    //
    ui32GPCfg <<= ui32GPCfgShft;

    AM_CRITICAL_BEGIN

    if (bClearEnable)
    {
        //
        // We're configuring a mode that requires clearing the Enable bit.
        //
        am_hal_gpio_output_tristate_disable(ui32Pin);
    }

    GPIO->PADKEY = GPIO_PADKEY_PADKEY_Key;

    // Here's where the magic happens
    AM_REGVAL(ui32GPCfgAddr) = (AM_REGVAL(ui32GPCfgAddr) & ui32GPCfgClearMask) | ui32GPCfg;

    GPIO->PADKEY = 0;

    AM_CRITICAL_END

    return AM_HAL_STATUS_SUCCESS;

} // am_hal_gpio_pinconfig()

/* Measures the length (in microseconds) of a pulse on the pin; state is HIGH
   or LOW, the type of pulse to measure.  Works on pulses from 2-3 microseconds
   to 3 minutes in length, but must be called at least a few dozen microseconds
   before the start of the pulse.

   Original Arduino function could operate in noInterrupt() context. This
   function cannot.
*/
unsigned long pulseIn(uint8_t pinNumber, uint8_t state, unsigned long timeout)
{
    return (pulseInLong(pinNumber, state, timeout));
}

/* Measures the length (in microseconds) of a pulse on the pin; state is HIGH
   or LOW, the type of pulse to measure.  Works on pulses from 2-3 microseconds
   to 3 minutes in length, but must be called at least a few dozen microseconds
   before the start of the pulse.

   ATTENTION: This function relies on micros() so cannot be used in noInterrupt() context
*/
unsigned long pulseInLong(uint8_t pinNumber, uint8_t state, unsigned long timeout)
{
    uint8_t padNumber = ap3_gpio_pin2pad(pinNumber);

    if (timeout > 3 * 60 * 1000000L)
        timeout = 3 * 60 * 1000000L; //Limit timeout to 3 minutes

    //Enable fast GPIO for this pad
    am_hal_gpio_fastgpio_disable(padNumber);
    am_hal_gpio_fastgpio_clr(padNumber);
    am_hal_gpio_fast_pinconfig((uint64_t)0x1 << padNumber, g_AM_HAL_GPIO_OUTPUT_WITH_READ, 0);

    uint32_t startMicros = micros();

    while (am_hal_gpio_fastgpio_read(padNumber) == state) //Wait for previous pulse to end
    {
        if (micros() - startMicros > timeout)
            return (0); //Pulse did not end
    }

    while (am_hal_gpio_fastgpio_read(padNumber) != state) //Wait for pin to change state
    {
        if (micros() - startMicros > timeout)
            return (0); //Pulse did not start
    }

    startMicros = micros(); //Restart time

    while (am_hal_gpio_fastgpio_read(padNumber) == state) //Wait for pin to exit sought state
    {
        if (micros() - startMicros > timeout)
            return (0); //Pulse did not end
    }

    uint32_t stopMicros = micros();

    am_hal_gpio_fastgpio_disable(padNumber);

    return (stopMicros - startMicros);
}