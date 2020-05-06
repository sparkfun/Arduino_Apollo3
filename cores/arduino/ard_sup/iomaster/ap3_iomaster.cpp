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

#include "ap3_iomaster.h"

IOMaster::IOMaster(uint8_t instance)
{
    _instance = instance;
    _handle = NULL;
}

ap3_err_t IOMaster::initialize(void)
{
    return initialize(_config);
}

ap3_err_t IOMaster::initialize(am_hal_iom_config_t config)
{
    uint32_t retVal32 = 0;
    _config = config;

    if (_handle != NULL)
    {
        deinitialize();
    }

    retVal32 = am_hal_iom_initialize(_instance, &_handle);
    if (retVal32 != AM_HAL_STATUS_SUCCESS)
    {
        return AP3_ERR;
    }

    retVal32 = am_hal_iom_power_ctrl(_handle, AM_HAL_SYSCTRL_WAKE, false);
    if (retVal32 != AM_HAL_STATUS_SUCCESS)
    {
        return AP3_ERR;
    }

    retVal32 = ap3_iom_configure(_handle, &_config);
    if (retVal32 != AM_HAL_STATUS_SUCCESS)
    {
        return AP3_ERR;
    }

    retVal32 = am_hal_iom_enable(_handle);
    if (retVal32 != AM_HAL_STATUS_SUCCESS)
    {
        return AP3_ERR;
    }

    // Configure the IOM pins. (Must be done by the inherited classes [this is just a reminder])

    return AP3_OK;
}

ap3_err_t IOMaster::deinitialize(void)
{
    uint32_t retVal32 = 0;

    if (_handle != NULL)
    {
        retVal32 = am_hal_iom_disable(_handle);
        if (retVal32 != AM_HAL_STATUS_SUCCESS)
        {
            return AP3_ERR;
        }

        retVal32 = am_hal_iom_power_ctrl(_handle, AM_HAL_SYSCTRL_DEEPSLEEP, false);
        if (retVal32 != AM_HAL_STATUS_SUCCESS)
        {
            return AP3_ERR;
        }

        retVal32 = am_hal_iom_uninitialize(_handle);
        if (retVal32 != AM_HAL_STATUS_SUCCESS)
        {
            return AP3_ERR;
        }
    }

    _handle = NULL;
    return AP3_OK;
}

ap3_err_t ap3_iom_pad_funcsel(uint8_t instance, ap3_iom_pad_type_e type, ap3_gpio_pad_t *pad, uint8_t *funcsel)
{
    ap3_err_t retval = AP3_OK;

    switch (instance)
    {
    case 0:
        switch (type)
        {
        case AP3_IOM_I2C_SCL:
            *pad = 5;
            *funcsel = AM_HAL_PIN_5_M0SCL;
            break;
        case AP3_IOM_I2C_SDA:
            *pad = 6;
            *funcsel = AM_HAL_PIN_6_M0SDAWIR3;
            break;
        case AP3_IOM_SPI_SCLK:
            *pad = 5;
            *funcsel = AM_HAL_PIN_5_M0SCK;
            break;
        case AP3_IOM_SPI_MOSI:
            *pad = 7;
            *funcsel = AM_HAL_PIN_7_M0MOSI;
            break;
        case AP3_IOM_SPI_MISO:
            *pad = 6;
            *funcsel = AM_HAL_PIN_6_M0MISO;
            break;
        default:
            goto invalid_args;
            break;
        }
        break;

    case 1:
        switch (type)
        {
        case AP3_IOM_I2C_SCL:
            *pad = 8;
            *funcsel = AM_HAL_PIN_8_M1SCL;
            break;
        case AP3_IOM_I2C_SDA:
            *pad = 9;
            *funcsel = AM_HAL_PIN_9_M1SDAWIR3;
            break;
        case AP3_IOM_SPI_SCLK:
            *pad = 8;
            *funcsel = AM_HAL_PIN_8_M1SCK;
            break;
        case AP3_IOM_SPI_MOSI:
            *pad = 10;
            *funcsel = AM_HAL_PIN_10_M1MOSI;
            break;
        case AP3_IOM_SPI_MISO:
            *pad = 9;
            *funcsel = AM_HAL_PIN_9_M1MISO;
            break;
        default:
            goto invalid_args;
            break;
        }
        break;

    case 2:
        switch (type)
        {
        case AP3_IOM_I2C_SCL:
            *pad = 27;
            *funcsel = AM_HAL_PIN_27_M2SCL;
            break;
        case AP3_IOM_I2C_SDA:
            *pad = 25;
            *funcsel = AM_HAL_PIN_25_M2SDAWIR3;
            break;
        case AP3_IOM_SPI_SCLK:
            *pad = 27;
            *funcsel = AM_HAL_PIN_27_M2SCK;
            break;
        case AP3_IOM_SPI_MOSI:
            *pad = 28;
            *funcsel = AM_HAL_PIN_28_M2MOSI;
            break;
        case AP3_IOM_SPI_MISO:
            *pad = 25;
            *funcsel = AM_HAL_PIN_25_M2MISO;
            break;
        default:
            goto invalid_args;
            break;
        }
        break;

    case 3:
        switch (type)
        {
        case AP3_IOM_I2C_SCL:
            *pad = 42;
            *funcsel = AM_HAL_PIN_42_M3SCL;
            break;
        case AP3_IOM_I2C_SDA:
            *pad = 43;
            *funcsel = AM_HAL_PIN_43_M3SDAWIR3;
            break;
        case AP3_IOM_SPI_SCLK:
            *pad = 42;
            *funcsel = AM_HAL_PIN_42_M3SCK;
            break;
        case AP3_IOM_SPI_MOSI:
            *pad = 38;
            *funcsel = AM_HAL_PIN_38_M3MOSI;
            break;
        case AP3_IOM_SPI_MISO:
            *pad = 43;
            *funcsel = AM_HAL_PIN_43_M3MISO;
            break;
        default:
            goto invalid_args;
            break;
        }
        break;

    case 4:
        switch (type)
        {
        case AP3_IOM_I2C_SCL:
            *pad = 39;
            *funcsel = AM_HAL_PIN_39_M4SCL;
            break;
        case AP3_IOM_I2C_SDA:
            *pad = 40;
            *funcsel = AM_HAL_PIN_40_M4SDAWIR3;
            break;
        case AP3_IOM_SPI_SCLK:
            *pad = 39;
            *funcsel = AM_HAL_PIN_39_M4SCK;
            break;
        case AP3_IOM_SPI_MOSI:
            *pad = 44;
            *funcsel = AM_HAL_PIN_44_M4MOSI;
            break;
        case AP3_IOM_SPI_MISO:
            *pad = 40;
            *funcsel = AM_HAL_PIN_40_M4MISO;
            break;
        default:
            goto invalid_args;
            break;
        }
        break;

    case 5:
        switch (type)
        {
        case AP3_IOM_I2C_SCL:
            *pad = 48;
            *funcsel = AM_HAL_PIN_48_M5SCL;
            break;
        case AP3_IOM_I2C_SDA:
            *pad = 49;
            *funcsel = AM_HAL_PIN_49_M5SDAWIR3;
            break;
        case AP3_IOM_SPI_SCLK:
            *pad = 48;
            *funcsel = AM_HAL_PIN_48_M5SCK;
            break;
        case AP3_IOM_SPI_MOSI:
            *pad = 47;
            *funcsel = AM_HAL_PIN_47_M5MOSI;
            break;
        case AP3_IOM_SPI_MISO:
            *pad = 49;
            *funcsel = AM_HAL_PIN_49_M5MISO;
            break;
        default:
            goto invalid_args;
            break;
        }
        break;

    default:
        goto invalid_args;
        break;
    }

    return retval;

invalid_args:
    retval = AP3_INVALID_ARG;
    *funcsel = 3; // do not use (GPIO)
    *pad = AP3_VARIANT_NUM_PINS;
    return retval;
}