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

#ifndef _AP3_UART_TYPES_H_
#define _AP3_UART_TYPES_H_

#define AP3_UART_PIN_UNUSED 0xFF

#define AP3_UART0_NUM_TX_PADS 12
#define AP3_UART0_NUM_RX_PADS 12
#define AP3_UART0_NUM_RTS_PADS 8
#define AP3_UART0_NUM_CTS_PADS 8

#define AP3_UART1_NUM_TX_PADS 12
#define AP3_UART1_NUM_RX_PADS 13
#define AP3_UART1_NUM_RTS_PADS 8
#define AP3_UART1_NUM_CTS_PADS 8

typedef enum
{
	AP3_UART_TX = 0x00,
	AP3_UART_RX,
	AP3_UART_RTS,
	AP3_UART_CTS,
} ap3_uart_pad_type_e;

typedef struct _ap3_uart_pad_map_elem_t
{
	ap3_gpio_pad_t pad;
	uint8_t funcsel;
} ap3_uart_pad_map_elem_t;

#endif // _AP3_UART_TYPES_H_