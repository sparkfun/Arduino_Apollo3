// #include "ap3_gpio.h"
#include "ap3_types.h"

// pad configurations
const am_hal_gpio_pincfg_t AP3_GPIO_PINCFG_NULL =
{
	.uFuncSel       = 0,    // [2:0]   Function select (FUNCSEL)
	.ePowerSw       = 0,    // [4:3]   Pin is a power switch source (VCC) or sink (VSS)
	.ePullup        = 0,    // [7:5]   Pin will enable a pullup resistor
	.eDriveStrength = 0,    // [9:8]   Pad strength designator
	.eGPOutcfg      = 0,    // [11:10] OUTCFG (GPIO config only)
	.eGPInput       = 0,    // [12:12] GPIO Input (GPIO config only)
	.eIntDir        = 0,    // [14:13] Interrupt direction
	.eGPRdZero      = 0,    // [15:15] GPIO read as zero

    //
    // The following descriptors designate the chip enable features of the
    // pin being configured.  If not a CE, these descriptors are ignored.
    // uIOMnum is 0-5 for the IOMs, or 6 for MSPI, 7 is invalid.
    //
	.uIOMnum        = 0,    // [18:16] IOM number (0-5), 6 for MSPI
	.uNCE           = 0,    // [20:19] NCE number (0-3).
	.eCEpol         = 0,    // [21:21] NCE polarity.

	.uRsvd22        = 0,    // [31:22]
};

const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_OUTPUT_WITH_READ_12 =
{
    .uFuncSel       = 3,
    .eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_12MA,
    .eGPOutcfg      = AM_HAL_GPIO_PIN_OUTCFG_PUSHPULL,
    .eGPInput       = AM_HAL_GPIO_PIN_INPUT_ENABLE,
    .eGPRdZero      = AM_HAL_GPIO_PIN_RDZERO_READPIN
};

const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_OPEN_DRAIN_WITH_READ_12 =
{
    .uFuncSel       = 3,
    .eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_12MA,
    .eGPOutcfg      = AM_HAL_GPIO_PIN_OUTCFG_OPENDRAIN,
    .eGPInput       = AM_HAL_GPIO_PIN_INPUT_ENABLE,
    .eGPRdZero      = AM_HAL_GPIO_PIN_RDZERO_READPIN
};

const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_INPUT_PULLDOWN =
{
    .uFuncSel       = 3,
    .eGPOutcfg      = AM_HAL_GPIO_PIN_OUTCFG_DISABLE,
    .eGPInput       = AM_HAL_GPIO_PIN_INPUT_ENABLE,
    .eGPRdZero      = AM_HAL_GPIO_PIN_RDZERO_READPIN,
    .ePullup        = AM_HAL_GPIO_PIN_PULLDOWN,
};