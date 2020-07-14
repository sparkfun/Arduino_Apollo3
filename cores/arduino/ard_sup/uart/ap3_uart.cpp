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
#include "ap3_uart.h"

#define AP3_NUM_UART 2

Uart *ap3_uart_handles[AP3_NUM_UART] = {0};
char ap3_g_prfbuf[AM_PRINTF_BUFSIZE];

Uart::Uart(uint8_t instance, ap3_gpio_pin_t pinRX, ap3_gpio_pin_t pinTX, ap3_gpio_pin_t pinRTS, ap3_gpio_pin_t pinCTS)
{
    _instance = instance;
    _handle = NULL;
    _config = ap3_uart_config_default;
    _pinRX = pinRX;
    _pinTX = pinTX;
    _pinRTS = pinRTS;
    _pinCTS = pinCTS;
}

void Uart::begin(unsigned long baudrate)
{
    begin(baudrate, (uint16_t)SERIAL_8N1);
    return;
}

void Uart::begin(unsigned long baudrate, uint16_t config)
{
    ap3_err_t retval = AP3_OK;
    retval = set_config((HardwareSerial_Config_e)config);
    _config.ui32BaudRate = baudrate;
    if (retval != AP3_OK)
    {
        return;
    }
    retval = _begin();
    return;
}

void Uart::begin(unsigned long baudrate, am_hal_uart_config_t config)
{
    ap3_err_t retval = AP3_OK;
    _config = config;
    _config.ui32BaudRate = baudrate;

    retval = _begin();
    return;
}

void Uart::end()
{
    if (_handle != NULL)
    {
        flush();

        // Power down the UART, and surrender the handle.
        am_hal_uart_power_control(_handle, AM_HAL_SYSCTRL_DEEPSLEEP, false);
        am_hal_uart_deinitialize(_handle);

        // Disable the UART pins.
        am_hal_gpio_pinconfig(_pinTX, g_AM_HAL_GPIO_DISABLE);
        am_hal_gpio_pinconfig(_pinRX, g_AM_HAL_GPIO_DISABLE);

        _handle = NULL;
    }
}

int Uart::available()
{
    return _rx_buffer.available();
}

int Uart::availableForWrite()
{
    return _tx_buffer.availableForStore();
}

int Uart::peek()
{
    return _rx_buffer.peek();
}

int Uart::read()
{
    return _rx_buffer.read_char();
}

void Uart::flush()
{
    // Make sure the UART has finished sending everything it's going to send.
    am_hal_uart_tx_flush(_handle);
}

size_t Uart::write(const uint8_t data)
{
    return write(&data, 1);
}

size_t Uart::write(const uint8_t *buffer, size_t size)
{
    uint32_t ui32TransferBytesWritten = 0;
    uint32_t ui32TotalBytesWritten = 0;

    //
    // Print the string via the UART.
    //
    am_hal_uart_transfer_t sUartWrite =
        {
            .ui32Direction = AM_HAL_UART_WRITE,
            .pui8Data = NULL,
            .ui32NumBytes = 0,
            .ui32TimeoutMs = 0,
            .pui32BytesTransferred = &ui32TransferBytesWritten,
        };

    do
    {
        while (!_tx_idle)
        {
        }; // wait for tx to become idle

        sUartWrite.pui8Data = (uint8_t *)((uint8_t *)buffer + ui32TotalBytesWritten);
        sUartWrite.ui32NumBytes = (size > AP3_UART_LINBUFF_SIZE) ? AP3_UART_LINBUFF_SIZE : size;
        am_hal_uart_transfer(_handle, &sUartWrite);

        ui32TotalBytesWritten += ui32TransferBytesWritten;

    } while (ui32TotalBytesWritten < size);

    if (ui32TotalBytesWritten != size)
    {
        //
        // Couldn't send the whole string!!
        //
        while (1)
        {
            digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
            delay(50);                       // wait for a second
            digitalWrite(LED_BUILTIN, LOW);  // turn the LED off by making the voltage LOW
            delay(50);
        }
    }

    return ui32TotalBytesWritten; //Return number of bytes pushed to UART hardware
}

// Stop Bits
#define AM_HAL_UART_ONE_STOP_BIT (_VAL2FLD(UART0_LCRH_STP2, 0))
#define AM_HAL_UART_TWO_STOP_BITS (_VAL2FLD(UART0_LCRH_STP2, 1))

ap3_err_t Uart::set_config(HardwareSerial_Config_e HWSconfig)
{
    ap3_err_t retval = AP3_OK;
    switch (HWSconfig)
    {
    case SERIAL_5N1:
        _config.ui32DataBits = AM_HAL_UART_DATA_BITS_5;
        _config.ui32Parity = AM_HAL_UART_PARITY_NONE;
        _config.ui32StopBits = AM_HAL_UART_ONE_STOP_BIT;
        break;
    case SERIAL_6N1:
        _config.ui32DataBits = AM_HAL_UART_DATA_BITS_6;
        _config.ui32Parity = AM_HAL_UART_PARITY_NONE;
        _config.ui32StopBits = AM_HAL_UART_ONE_STOP_BIT;
        break;
    case SERIAL_7N1:
        _config.ui32DataBits = AM_HAL_UART_DATA_BITS_7;
        _config.ui32Parity = AM_HAL_UART_PARITY_NONE;
        _config.ui32StopBits = AM_HAL_UART_ONE_STOP_BIT;
        break;
    case SERIAL_8N1:
        _config.ui32DataBits = AM_HAL_UART_DATA_BITS_8;
        _config.ui32Parity = AM_HAL_UART_PARITY_NONE;
        _config.ui32StopBits = AM_HAL_UART_ONE_STOP_BIT;
        break;
    case SERIAL_5N2:
        _config.ui32DataBits = AM_HAL_UART_DATA_BITS_5;
        _config.ui32Parity = AM_HAL_UART_PARITY_NONE;
        _config.ui32StopBits = AM_HAL_UART_TWO_STOP_BITS;
        break;
    case SERIAL_6N2:
        _config.ui32DataBits = AM_HAL_UART_DATA_BITS_6;
        _config.ui32Parity = AM_HAL_UART_PARITY_NONE;
        _config.ui32StopBits = AM_HAL_UART_TWO_STOP_BITS;
        break;
    case SERIAL_7N2:
        _config.ui32DataBits = AM_HAL_UART_DATA_BITS_7;
        _config.ui32Parity = AM_HAL_UART_PARITY_NONE;
        _config.ui32StopBits = AM_HAL_UART_TWO_STOP_BITS;
        break;
    case SERIAL_8N2:
        _config.ui32DataBits = AM_HAL_UART_DATA_BITS_8;
        _config.ui32Parity = AM_HAL_UART_PARITY_NONE;
        _config.ui32StopBits = AM_HAL_UART_TWO_STOP_BITS;
        break;
    case SERIAL_5E1:
        _config.ui32DataBits = AM_HAL_UART_DATA_BITS_5;
        _config.ui32Parity = AM_HAL_UART_PARITY_EVEN;
        _config.ui32StopBits = AM_HAL_UART_ONE_STOP_BIT;
        break;
    case SERIAL_6E1:
        _config.ui32DataBits = AM_HAL_UART_DATA_BITS_6;
        _config.ui32Parity = AM_HAL_UART_PARITY_EVEN;
        _config.ui32StopBits = AM_HAL_UART_ONE_STOP_BIT;
        break;
    case SERIAL_7E1:
        _config.ui32DataBits = AM_HAL_UART_DATA_BITS_7;
        _config.ui32Parity = AM_HAL_UART_PARITY_EVEN;
        _config.ui32StopBits = AM_HAL_UART_ONE_STOP_BIT;
        break;
    case SERIAL_8E1:
        _config.ui32DataBits = AM_HAL_UART_DATA_BITS_8;
        _config.ui32Parity = AM_HAL_UART_PARITY_EVEN;
        _config.ui32StopBits = AM_HAL_UART_ONE_STOP_BIT;
        break;
    case SERIAL_5E2:
        _config.ui32DataBits = AM_HAL_UART_DATA_BITS_5;
        _config.ui32Parity = AM_HAL_UART_PARITY_EVEN;
        _config.ui32StopBits = AM_HAL_UART_TWO_STOP_BITS;
        break;
    case SERIAL_6E2:
        _config.ui32DataBits = AM_HAL_UART_DATA_BITS_6;
        _config.ui32Parity = AM_HAL_UART_PARITY_EVEN;
        _config.ui32StopBits = AM_HAL_UART_TWO_STOP_BITS;
        break;
    case SERIAL_7E2:
        _config.ui32DataBits = AM_HAL_UART_DATA_BITS_7;
        _config.ui32Parity = AM_HAL_UART_PARITY_EVEN;
        _config.ui32StopBits = AM_HAL_UART_TWO_STOP_BITS;
        break;
    case SERIAL_8E2:
        _config.ui32DataBits = AM_HAL_UART_DATA_BITS_8;
        _config.ui32Parity = AM_HAL_UART_PARITY_EVEN;
        _config.ui32StopBits = AM_HAL_UART_TWO_STOP_BITS;
        break;
    case SERIAL_5O1:
        _config.ui32DataBits = AM_HAL_UART_DATA_BITS_5;
        _config.ui32Parity = AM_HAL_UART_PARITY_ODD;
        _config.ui32StopBits = AM_HAL_UART_ONE_STOP_BIT;
        break;
    case SERIAL_6O1:
        _config.ui32DataBits = AM_HAL_UART_DATA_BITS_6;
        _config.ui32Parity = AM_HAL_UART_PARITY_ODD;
        _config.ui32StopBits = AM_HAL_UART_ONE_STOP_BIT;
        break;
    case SERIAL_7O1:
        _config.ui32DataBits = AM_HAL_UART_DATA_BITS_7;
        _config.ui32Parity = AM_HAL_UART_PARITY_ODD;
        _config.ui32StopBits = AM_HAL_UART_ONE_STOP_BIT;
        break;
    case SERIAL_8O1:
        _config.ui32DataBits = AM_HAL_UART_DATA_BITS_8;
        _config.ui32Parity = AM_HAL_UART_PARITY_ODD;
        _config.ui32StopBits = AM_HAL_UART_ONE_STOP_BIT;
        break;
    case SERIAL_5O2:
        _config.ui32DataBits = AM_HAL_UART_DATA_BITS_5;
        _config.ui32Parity = AM_HAL_UART_PARITY_ODD;
        _config.ui32StopBits = AM_HAL_UART_TWO_STOP_BITS;
        break;
    case SERIAL_6O2:
        _config.ui32DataBits = AM_HAL_UART_DATA_BITS_6;
        _config.ui32Parity = AM_HAL_UART_PARITY_ODD;
        _config.ui32StopBits = AM_HAL_UART_TWO_STOP_BITS;
        break;
    case SERIAL_7O2:
        _config.ui32DataBits = AM_HAL_UART_DATA_BITS_7;
        _config.ui32Parity = AM_HAL_UART_PARITY_ODD;
        _config.ui32StopBits = AM_HAL_UART_TWO_STOP_BITS;
        break;
    case SERIAL_8O2:
        _config.ui32DataBits = AM_HAL_UART_DATA_BITS_8;
        _config.ui32Parity = AM_HAL_UART_PARITY_ODD;
        _config.ui32StopBits = AM_HAL_UART_TWO_STOP_BITS;
        break;
    default:
        retval = AP3_INVALID_ARG;
        break;
    }

    //Setup flow control
    _config.ui32FlowControl = AM_HAL_UART_FLOW_CTRL_NONE;
    if(_pinRTS != AP3_UART_PIN_UNUSED && _pinCTS != AP3_UART_PIN_UNUSED)
    {
        _config.ui32FlowControl = AM_HAL_UART_FLOW_CTRL_RTS_CTS;
    }
    else if(_pinRTS != AP3_UART_PIN_UNUSED)
    {
        _config.ui32FlowControl = AM_HAL_UART_FLOW_CTRL_RTS_ONLY;
    }
    else if(_pinCTS != AP3_UART_PIN_UNUSED)
    {
        _config.ui32FlowControl = AM_HAL_UART_FLOW_CTRL_CTS_ONLY;
    }

    return retval;
}

ap3_err_t Uart::_begin(void)
{
    ap3_err_t retval = AP3_OK;
    am_hal_gpio_pincfg_t pincfg = AP3_GPIO_DEFAULT_PINCFG;
    uint8_t funcsel = 0;

    // Link in the buffers for the HAL
    _config.pui8RxBuffer = _rx_linbuff;
    _config.pui8TxBuffer = _tx_linbuff;
    _config.ui32RxBufferSize = sizeof(_rx_linbuff);
    _config.ui32TxBufferSize = sizeof(_tx_linbuff);

    //User may want to change settings mid-sketch. Only init UART if it's new.
    if (_handle == NULL)
    {
        retval = (ap3_err_t)am_hal_uart_initialize(_instance, &_handle);
        if (retval != AP3_OK)
        {
            return ap3_return(retval);
        }
    }
    retval = (ap3_err_t)am_hal_uart_power_control(_handle, AM_HAL_SYSCTRL_WAKE, false);
    if (retval != AP3_OK)
    {
        return ap3_return(retval);
    }
    retval = (ap3_err_t)am_hal_uart_configure(_handle, &_config);
    if (retval != AP3_OK)
    {
        return ap3_return(retval);
    }

    UARTn(_instance)->LCRH_b.FEN = 0; // Disable that pesky FIFO

    // Check for a valid instance
    // Check pins for compatibility with the selcted instance

    if ((_pinRX == AP3_UART_PIN_UNUSED) && (_pinTX == AP3_UART_PIN_UNUSED) && (_pinRTS == AP3_UART_PIN_UNUSED) && (_pinCTS == AP3_UART_PIN_UNUSED))
    {
        return AP3_ERR; // must provide at least one pin
    }

    if (_pinTX != AP3_UART_PIN_UNUSED)
    {
        retval = ap3_uart_pad_funcsel(_instance, AP3_UART_TX, ap3_gpio_pin2pad(_pinTX), &funcsel);
        if (retval != AP3_OK)
        {
            return retval;
        }
        pincfg.uFuncSel = funcsel; // set the proper function select option for this instance/pin/type combination
        pinMode(_pinTX, pincfg, &retval);
        if (retval != AP3_OK)
        {
            return ap3_return(retval);
        }
        pincfg = AP3_GPIO_DEFAULT_PINCFG; // set back to default for use with next pin
    }

    if (_pinRX != AP3_UART_PIN_UNUSED)
    {
        retval = ap3_uart_pad_funcsel(_instance, AP3_UART_RX, ap3_gpio_pin2pad(_pinRX), &funcsel);
        if (retval != AP3_OK)
        {
            return retval;
        }
        pincfg.uFuncSel = funcsel; // set the proper function select option for this instance/pin/type combination
        pinMode(_pinRX, pincfg, &retval);
        if (retval != AP3_OK)
        {
            return ap3_return(retval);
        }
        pincfg = AP3_GPIO_DEFAULT_PINCFG; // set back to default for use with next pin
    }

    if (_pinRTS != AP3_UART_PIN_UNUSED)
    {
        retval = ap3_uart_pad_funcsel(_instance, AP3_UART_RTS, ap3_gpio_pin2pad(_pinRTS), &funcsel);
        if (retval != AP3_OK)
        {
            return retval;
        }
        pincfg.uFuncSel = funcsel; // set the proper function select option for this instance/pin/type combination
        pinMode(_pinRTS, pincfg, &retval);
        if (retval != AP3_OK)
        {
            return ap3_return(retval);
        }
        pincfg = AP3_GPIO_DEFAULT_PINCFG; // set back to default for use with next pin
    }

    if (_pinCTS != AP3_UART_PIN_UNUSED)
    {
        retval = ap3_uart_pad_funcsel(_instance, AP3_UART_CTS, ap3_gpio_pin2pad(_pinCTS), &funcsel);
        if (retval != AP3_OK)
        {
            return retval;
        }
        pincfg.uFuncSel = funcsel; // set the proper function select option for this instance/pin/type combination
        pinMode(_pinCTS, pincfg, &retval);
        if (retval != AP3_OK)
        {
            return ap3_return(retval);
        }
        pincfg = AP3_GPIO_DEFAULT_PINCFG; // set back to default for use with next pin
    }

    // Enable TX and RX interrupts
    NVIC_EnableIRQ((IRQn_Type)(UART0_IRQn + _instance));
    am_hal_uart_interrupt_enable(_handle, (AM_HAL_UART_INT_RX | AM_HAL_UART_INT_TX));
    am_hal_interrupt_master_enable();

    // Service interrupts to determine idle state
    am_hal_uart_interrupt_service(_handle, 0, (uint32_t *)&_tx_idle);

    // Register the class into the local list
    ap3_uart_handles[_instance] = this;

    return retval;
}

uint32_t Uart::printf(const char *pcFmt, ...)
{
    uint32_t ui32NumChars;

    //
    // Convert to the desired string.
    //
    va_list pArgs;
    va_start(pArgs, pcFmt);
    ui32NumChars = am_util_stdio_vsprintf(ap3_g_prfbuf, pcFmt, pArgs);
    va_end(pArgs);

    write((const uint8_t *)ap3_g_prfbuf, ui32NumChars);

    return ui32NumChars;
}

ap3_err_t ap3_uart_pad_funcsel(uint8_t instance, ap3_uart_pad_type_e type, ap3_gpio_pad_t pad, uint8_t *funcsel)
{
    ap3_err_t retval = AP3_ERR;

    const ap3_uart_pad_map_elem_t *map = NULL;
    uint8_t map_len = 0;
    switch (instance)
    {
    case 0:
        switch (type)
        {
        case AP3_UART_TX:
            map = ap3_uart0_tx_map;
            map_len = AP3_UART0_NUM_TX_PADS;
            break;
        case AP3_UART_RX:
            map = ap3_uart0_rx_map;
            map_len = AP3_UART0_NUM_RX_PADS;
            break;
        case AP3_UART_RTS:
            map = ap3_uart0_rts_map;
            map_len = AP3_UART0_NUM_RTS_PADS;
            break;
        case AP3_UART_CTS:
            map = ap3_uart0_cts_map;
            map_len = AP3_UART0_NUM_CTS_PADS;
            break;
        default:
            goto invalid_args;
            break;
        }
        break;

    case 1:
        switch (type)
        {
        case AP3_UART_TX:
            map = ap3_uart1_tx_map;
            map_len = AP3_UART1_NUM_TX_PADS;
            break;
        case AP3_UART_RX:
            map = ap3_uart1_rx_map;
            map_len = AP3_UART1_NUM_RX_PADS;
            break;
        case AP3_UART_RTS:
            map = ap3_uart1_rts_map;
            map_len = AP3_UART1_NUM_RTS_PADS;
            break;
        case AP3_UART_CTS:
            map = ap3_uart1_cts_map;
            map_len = AP3_UART1_NUM_CTS_PADS;
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

    if ((map == NULL) || (map_len == 0))
    {
        goto invalid_args;
    }

    for (uint16_t indi = 0; indi < map_len; indi++)
    {
        if (map[indi].pad == pad)
        {
            *funcsel = map[indi].funcsel;
            return AP3_OK;
        }
    }
    return retval;

invalid_args:
    retval = AP3_INVALID_ARG;
    *funcsel = 0; // do not use
    return retval;
}

//*****************************************************************************
//
// Interrupt handler for the UART.
//
//*****************************************************************************
inline void Uart::uart_isr(void)
{
    uint32_t ui32Status;

    // Service the FIFOs as necessary, and clear the interrupts.
    am_hal_uart_interrupt_status_get(_handle, &ui32Status, true);
    am_hal_uart_interrupt_clear(_handle, ui32Status);
    am_hal_uart_interrupt_service(_handle, ui32Status, (uint32_t *)&_tx_idle);

    if (ui32Status & AM_HAL_UART_INT_RX)
    {
        uint32_t ui32BytesRead = 0x00;
        uint8_t rx_c = 0x00;

        am_hal_uart_transfer_t sRead =
            {
                .ui32Direction = AM_HAL_UART_READ,
                .pui8Data = (uint8_t *)&rx_c,
                .ui32NumBytes = sizeof(rx_c),
                .ui32TimeoutMs = 0,
                .pui32BytesTransferred = &ui32BytesRead,
            };

        am_hal_uart_transfer(_handle, &sRead);
        if (ui32BytesRead)
        {
            _rx_buffer.store_char(rx_c);
        }
    }
}

// Individual ISR implementations for the two UART peripherals on the Apollo3
extern "C" void am_uart_isr(void)
{
    if (ap3_uart_handles[0] != NULL)
    {
        ap3_uart_handles[0]->uart_isr();
    }
}

extern "C" void am_uart1_isr(void)
{
    if (ap3_uart_handles[1] != NULL)
    {
        ap3_uart_handles[1]->uart_isr();
    }
}
