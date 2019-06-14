#include "ap3_initialization.h"

// If the bsp low power init function exists we want to use it at startup
void am_bsp_low_power_init(void) __attribute__((weak));
void am_bsp_low_power_init(void) {}

void ap3_init( void ){
    // Set the clock frequency.
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);

    // Set the default cache configuration
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    // Configure the board for low power operation.
    am_bsp_low_power_init();

    // Configure the stimer
    am_hal_stimer_int_enable(AM_HAL_STIMER_INT_OVERFLOW);
    NVIC_EnableIRQ(STIMER_IRQn);
    am_hal_stimer_config(AM_HAL_STIMER_CFG_CLEAR | AM_HAL_STIMER_CFG_FREEZE);
    am_hal_stimer_config(AM_HAL_STIMER_HFRC_3MHZ);

    // Enable interruts master
    am_hal_interrupt_master_enable();
}