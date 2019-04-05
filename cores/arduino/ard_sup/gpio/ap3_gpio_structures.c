#include "ap3_gpio.h"

// pad configurations
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