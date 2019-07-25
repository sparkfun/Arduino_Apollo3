#include "svl_utils.h"

#define AP3_STIMER_FREQ_HZ (3000000)
#define AP3_STIMER_FREQ_KHZ (AP3_STIMER_FREQ_HZ / 1000)
#define AP3_STIMER_FREQ_MHZ (AP3_STIMER_FREQ_HZ / 1000000)

volatile uint32_t ap3_stimer_overflows = 0x00;
uint64_t ticks = 0;

void _fill_ticks(void)
{
    ticks = ap3_stimer_overflows;
    ticks <<= 32;
    ticks |= (am_hal_stimer_counter_get() & 0xFFFFFFFF);
}

size_t millis(void){
    _fill_ticks();
    return (uint32_t)(ticks / AP3_STIMER_FREQ_KHZ);
}


//*****************************************************************************
//
// Burst mode
//
//*****************************************************************************
bool enable_burst_mode(void)
{
    // Check that the Burst Feature is available.
    am_hal_burst_avail_e eBurstModeAvailable;
    if (AM_HAL_STATUS_SUCCESS != am_hal_burst_mode_initialize(&eBurstModeAvailable))
    {
        return (false);
    }

    // Put the MCU into "Burst" mode.
    am_hal_burst_mode_e eBurstMode;
    if (AM_HAL_STATUS_SUCCESS != am_hal_burst_mode_enable(&eBurstMode))
    {
        return (false);
    }
    return (true);
}

//Turns main processor from 96MHz to 48MHz
//Returns false if disable fails
bool disable_burst_mode(void)
{
    am_hal_burst_mode_e eBurstMode;
    if (AM_HAL_STATUS_SUCCESS == am_hal_burst_mode_disable(&eBurstMode))
    {
        if (AM_HAL_NORMAL_MODE != eBurstMode)
        {
            return (false);
        }
    }
    else
    {
        return (false);
    }
    return (true);
}



//*****************************************************************************
//  Local defines. Copied from am_hal_gpio.c
//*****************************************************************************
//
// Generally define GPIO PADREG and GPIOCFG bitfields
//
#define PADREG_FLD_76_S 6
#define PADREG_FLD_FNSEL_S 3
#define PADREG_FLD_DRVSTR_S 2
#define PADREG_FLD_INPEN_S 1
#define PADREG_FLD_PULLUP_S 0

#define GPIOCFG_FLD_INTD_S 3
#define GPIOCFG_FLD_OUTCFG_S 1
#define GPIOCFG_FLD_INCFG_S 0

uint32_t ap3_gpio_enable_interrupts(uint32_t ui32Pin, uint32_t eIntDir){
    uint32_t ui32Padreg, ui32AltPadCfg, ui32GPCfg;
    bool bClearEnable = false;


    ui32GPCfg = ui32Padreg = ui32AltPadCfg = 0;
    ui32GPCfg |= (((eIntDir >> 0) & 0x1) << GPIOCFG_FLD_INTD_S) | (((eIntDir >> 1) & 0x1) << GPIOCFG_FLD_INCFG_S);

    uint32_t ui32GPCfgAddr;
    uint32_t ui32GPCfgClearMask;
    uint32_t ui32GPCfgShft;

    ui32GPCfgShft       = ((ui32Pin & 0x7) << 2);

    ui32GPCfgAddr = AM_REGADDR(GPIO, CFGA) + ((ui32Pin >> 1) & ~0x3);
    ui32GPCfgClearMask = ~((uint32_t)0xF << ui32GPCfgShft);

    ui32GPCfg <<= ui32GPCfgShft;

    AM_CRITICAL_BEGIN

    if (bClearEnable)
    {
        am_hal_gpio_output_tristate_disable(ui32Pin);
    }

    GPIO->PADKEY = GPIO_PADKEY_PADKEY_Key;

    // Here's where the magic happens
    AM_REGVAL(ui32GPCfgAddr) = (AM_REGVAL(ui32GPCfgAddr) & ui32GPCfgClearMask) | ui32GPCfg;

    GPIO->PADKEY = 0;

    AM_CRITICAL_END

    return AM_HAL_STATUS_SUCCESS;
}
