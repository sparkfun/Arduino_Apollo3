#include "BurstMode.h"

uint32_t cpuFreq = 48000000; //At POR core is 48MHz

//Turns main processor from 48MHz to 96MHz
//Returns false if burst mode failed to enable
bool enableBurstMode(void)
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
    cpuFreq = 96000000;
    return (true);
}

//Turns main processor from 96MHz to 48MHz
//Returns false if disable fails
bool disableBurstMode(void)
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
    cpuFreq = 48000000;
    return (true);
}

//Returns the current core speed
uint32_t getCpuFreqMHz(void)
{
    return (cpuFreq);
}