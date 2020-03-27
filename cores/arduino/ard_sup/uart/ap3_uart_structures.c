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

// #include "ap3_uart.h"
#include "ap3_types.h"
#include "ap3_uart_types.h"

const ap3_uart_pad_map_elem_t ap3_uart0_tx_map[AP3_UART0_NUM_TX_PADS] = {
    {.pad = 1, .funcsel = AM_HAL_PIN_1_UART0TX},
    {.pad = 7, .funcsel = AM_HAL_PIN_7_UART0TX},
    {.pad = 16, .funcsel = AM_HAL_PIN_16_UART0TX},
    {.pad = 20, .funcsel = AM_HAL_PIN_20_UART0TX},
    {.pad = 22, .funcsel = AM_HAL_PIN_22_UART0TX},
    {.pad = 26, .funcsel = AM_HAL_PIN_26_UART0TX},
    {.pad = 28, .funcsel = AM_HAL_PIN_28_UART0TX},
    {.pad = 30, .funcsel = AM_HAL_PIN_30_UART0TX},
    {.pad = 39, .funcsel = AM_HAL_PIN_39_UART0TX},
    {.pad = 41, .funcsel = AM_HAL_PIN_41_UART0TX},
	{.pad = 44, .funcsel = AM_HAL_PIN_44_UART0TX},
    {.pad = 48, .funcsel = AM_HAL_PIN_48_UART0TX},
};

const ap3_uart_pad_map_elem_t ap3_uart0_rx_map[AP3_UART0_NUM_RX_PADS] = {
    {.pad = 2, .funcsel = AM_HAL_PIN_2_UART0RX},
    {.pad = 11, .funcsel = AM_HAL_PIN_11_UART0RX},
    {.pad = 17, .funcsel = AM_HAL_PIN_17_UART0RX},
    {.pad = 21, .funcsel = AM_HAL_PIN_21_UART0RX},
    {.pad = 23, .funcsel = AM_HAL_PIN_23_UART0RX},
    {.pad = 27, .funcsel = AM_HAL_PIN_27_UART0RX},
    {.pad = 29, .funcsel = AM_HAL_PIN_29_UART0RX},
    {.pad = 31, .funcsel = AM_HAL_PIN_31_UART0RX},
    {.pad = 34, .funcsel = AM_HAL_PIN_34_UART0RX},
    {.pad = 40, .funcsel = AM_HAL_PIN_40_UART0RX},
    {.pad = 45, .funcsel = AM_HAL_PIN_45_UART0RX},
    {.pad = 49, .funcsel = AM_HAL_PIN_49_UART0RX},
};

const ap3_uart_pad_map_elem_t ap3_uart0_rts_map[AP3_UART0_NUM_RTS_PADS] = {
    {.pad = 3, .funcsel = AM_HAL_PIN_3_UART0RTS},
    {.pad = 5, .funcsel = AM_HAL_PIN_5_UART0RTS},
    {.pad = 13, .funcsel = AM_HAL_PIN_13_UART0RTS},
    {.pad = 18, .funcsel = AM_HAL_PIN_18_UART0RTS},
    {.pad = 34, .funcsel = AM_HAL_PIN_34_UART0RTS},
    {.pad = 35, .funcsel = AM_HAL_PIN_35_UART0RTS},
    {.pad = 37, .funcsel = AM_HAL_PIN_37_UART0RTS},
    {.pad = 41, .funcsel = AM_HAL_PIN_41_UART0RTS},
};

const ap3_uart_pad_map_elem_t ap3_uart0_cts_map[AP3_UART0_NUM_CTS_PADS] = {
    {.pad = 4, .funcsel = AM_HAL_PIN_4_UART0CTS},
    {.pad = 6, .funcsel = AM_HAL_PIN_6_UART0CTS},
    {.pad = 12, .funcsel = AM_HAL_PIN_12_UART0CTS},
    {.pad = 24, .funcsel = AM_HAL_PIN_24_UART0CTS},
    {.pad = 29, .funcsel = AM_HAL_PIN_29_UART0CTS},
    {.pad = 33, .funcsel = AM_HAL_PIN_33_UART0CTS},
	{.pad = 36, .funcsel = AM_HAL_PIN_36_UART0CTS},
    {.pad = 38, .funcsel = AM_HAL_PIN_38_UART0CTS},
};

const ap3_uart_pad_map_elem_t ap3_uart1_tx_map[AP3_UART1_NUM_TX_PADS] = {
    {.pad = 8, .funcsel = AM_HAL_PIN_8_UART1TX},
	{.pad = 10, .funcsel = AM_HAL_PIN_10_UART1TX},
    {.pad = 12, .funcsel = AM_HAL_PIN_12_UART1TX},
    {.pad = 14, .funcsel = AM_HAL_PIN_14_UART1TX},
    {.pad = 18, .funcsel = AM_HAL_PIN_18_UART1TX},
    {.pad = 20, .funcsel = AM_HAL_PIN_20_UART1TX},
    {.pad = 24, .funcsel = AM_HAL_PIN_24_UART1TX},
    {.pad = 35, .funcsel = AM_HAL_PIN_35_UART1TX},
    {.pad = 37, .funcsel = AM_HAL_PIN_37_UART1TX},
    {.pad = 39, .funcsel = AM_HAL_PIN_39_UART1TX},
    {.pad = 42, .funcsel = AM_HAL_PIN_42_UART1TX},
    {.pad = 46, .funcsel = AM_HAL_PIN_46_UART1TX},
};

const ap3_uart_pad_map_elem_t ap3_uart1_rx_map[AP3_UART1_NUM_RX_PADS] = {
    {.pad = 2, .funcsel = AM_HAL_PIN_2_UART1RX},
	{.pad = 4, .funcsel = AM_HAL_PIN_4_UART1RX},
	{.pad = 9, .funcsel = AM_HAL_PIN_9_UART1RX},
    {.pad = 13, .funcsel = AM_HAL_PIN_13_UART1RX},
    {.pad = 15, .funcsel = AM_HAL_PIN_15_UART1RX},
    {.pad = 19, .funcsel = AM_HAL_PIN_19_UART1RX},
    {.pad = 21, .funcsel = AM_HAL_PIN_21_UART1RX},
    {.pad = 25, .funcsel = AM_HAL_PIN_25_UART1RX},
    {.pad = 36, .funcsel = AM_HAL_PIN_36_UART1RX},
    {.pad = 38, .funcsel = AM_HAL_PIN_38_UART1RX},
    {.pad = 40, .funcsel = AM_HAL_PIN_40_UART1RX},
    {.pad = 43, .funcsel = AM_HAL_PIN_43_UART1RX},
    {.pad = 47, .funcsel = AM_HAL_PIN_47_UART1RX},
};

const ap3_uart_pad_map_elem_t ap3_uart1_rts_map[AP3_UART1_NUM_RTS_PADS] = {
    {.pad = 10, .funcsel = AM_HAL_PIN_10_UART1RTS},
    {.pad = 16, .funcsel = AM_HAL_PIN_16_UART1RTS},
    {.pad = 20, .funcsel = AM_HAL_PIN_20_UART1RTS},
    {.pad = 30, .funcsel = AM_HAL_PIN_30_UART1RTS},
    {.pad = 31, .funcsel = AM_HAL_PIN_31_UART1RTS},
    {.pad = 34, .funcsel = AM_HAL_PIN_34_UART1RTS},
    {.pad = 41, .funcsel = AM_HAL_PIN_41_UART1RTS},
    {.pad = 44, .funcsel = AM_HAL_PIN_44_UART1RTS},
};

const ap3_uart_pad_map_elem_t ap3_uart1_cts_map[AP3_UART1_NUM_CTS_PADS] = {
    {.pad = 11, .funcsel = AM_HAL_PIN_11_UART1CTS},
    {.pad = 17, .funcsel = AM_HAL_PIN_17_UART1CTS},
    {.pad = 21, .funcsel = AM_HAL_PIN_21_UART1CTS},
    {.pad = 26, .funcsel = AM_HAL_PIN_26_UART1CTS},
    {.pad = 29, .funcsel = AM_HAL_PIN_29_UART1CTS},
    {.pad = 32, .funcsel = AM_HAL_PIN_32_UART1CTS},
    {.pad = 36, .funcsel = AM_HAL_PIN_36_UART1CTS},
    {.pad = 45, .funcsel = AM_HAL_PIN_45_UART1CTS},
};

const am_hal_uart_config_t ap3_uart_config_default = {
    //
    // Standard UART options.
    //
    .ui32BaudRate = 115200,
    .ui32DataBits = AM_HAL_UART_DATA_BITS_8,
    .ui32Parity = AM_HAL_UART_PARITY_NONE,
    .ui32StopBits = AM_HAL_UART_ONE_STOP_BIT,
    .ui32FlowControl = AM_HAL_UART_FLOW_CTRL_NONE,

    //
    // Additional options.
    //
    .ui32FifoLevels = (AM_HAL_UART_TX_FIFO_1_2 |
                       AM_HAL_UART_RX_FIFO_1_2),

    //
    // Buffers
    //
    .pui8TxBuffer = 0,
    .ui32TxBufferSize = 0,
    .pui8RxBuffer = 0,
    .ui32RxBufferSize = 0,
};