#include "structs.h"

const am_hal_gpio_pincfg_t g_deepsleep_button0 =
{
    .ePullup = AM_HAL_GPIO_PIN_PULLUP_WEAK,
    .uFuncSel = 3,
    .eIntDir = AM_HAL_GPIO_PIN_INTDIR_LO2HI,
    .eGPInput = AM_HAL_GPIO_PIN_INPUT_ENABLE,
};
