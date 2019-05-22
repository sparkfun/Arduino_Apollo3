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

#ifndef _AP3_UART_H_
#define _AP3_UART_H_

#include "Arduino.h"
#include "ap3_uart_types.h"

#include "HardwareSerial.h"
#include "RingBuffer.h"

#ifndef AP3_UART_RINGBUFF_SIZE
#define AP3_UART_RINGBUFF_SIZE 256
#endif

typedef RingBufferN<AP3_UART_RINGBUFF_SIZE> AP3UartRingBuffer;

extern const am_hal_uart_config_t ap3_uart_config_default;

extern const ap3_uart_pad_map_elem_t ap3_uart0_tx_map[AP3_UART0_NUM_TX_PADS];
extern const ap3_uart_pad_map_elem_t ap3_uart0_rx_map[AP3_UART0_NUM_RX_PADS];
extern const ap3_uart_pad_map_elem_t ap3_uart0_rts_map[AP3_UART0_NUM_RTS_PADS];
extern const ap3_uart_pad_map_elem_t ap3_uart0_cts_map[AP3_UART0_NUM_CTS_PADS];

extern const ap3_uart_pad_map_elem_t ap3_uart1_tx_map[AP3_UART1_NUM_TX_PADS];
extern const ap3_uart_pad_map_elem_t ap3_uart1_rx_map[AP3_UART1_NUM_RX_PADS];
extern const ap3_uart_pad_map_elem_t ap3_uart1_rts_map[AP3_UART1_NUM_RTS_PADS];
extern const ap3_uart_pad_map_elem_t ap3_uart1_cts_map[AP3_UART1_NUM_CTS_PADS];

ap3_err_t ap3_uart_pad_funcsel(uint8_t instance, ap3_uart_pad_type_e type, ap3_gpio_pad_t pad, uint8_t *funcsel);

class Uart : public HardwareSerial
{
public:
	Uart(uint8_t instance, ap3_gpio_pin_t pinRX, ap3_gpio_pin_t pinTX, ap3_gpio_pin_t pinRTS = AP3_UART_PIN_UNUSED, ap3_gpio_pin_t pinCTS = AP3_UART_PIN_UNUSED);
	void begin(unsigned long baudRate);
	void begin(unsigned long baudrate, uint16_t config);
	void begin(unsigned long baudrate, am_hal_uart_config_t config);
	void end();
	int available();
	int availableForWrite();
	int peek();
	int read();
	void flush();
	size_t write(const uint8_t data);
	size_t write(const uint8_t *buffer, size_t size);
	uint32_t printf(const char *pcFmt, ...);
	using Print::write; // pull in write(str) and write(buf, size) from Print

	void rx_isr(void);

	operator bool() { return true; } // todo: wait for a serial terminal to be open... probably depends on RTS or CTS...

private:
public:							  //temporary
	AP3UartRingBuffer _rx_buffer; // These buffers guarantee the lifespan of the data to transmit
	AP3UartRingBuffer _tx_buffer; //		to allow for asynchronous tranfsers

	uint8_t _instance;
	void *_handle;
	am_hal_uart_config_t _config;
	ap3_gpio_pin_t _pinRX;
	ap3_gpio_pin_t _pinTX;
	ap3_gpio_pin_t _pinRTS;
	ap3_gpio_pin_t _pinCTS;

	ap3_err_t set_config(HardwareSerial_Config_e HWSconfig);
	ap3_err_t _begin(void); // call once all members + config structure are set up

	// ap3_err_t initialize( void );

	// SERCOM *sercom;
	// RingBuffer rxBuffer;
	// RingBuffer txBuffer;

	// uint8_t uc_pinRX;
	// uint8_t uc_pinTX;
	// SercomRXPad uc_padRX;
	// SercomUartTXPad uc_padTX;
	// uint8_t uc_pinRTS;
	// volatile uint32_t* pul_outsetRTS;
	// volatile uint32_t* pul_outclrRTS;
	// uint32_t ul_pinMaskRTS;
	// uint8_t uc_pinCTS;

	// SercomNumberStopBit extractNbStopBit(uint16_t config);
	// SercomUartCharSize extractCharSize(uint16_t config);
	// SercomParityMode extractParity(uint16_t config);
};

#endif // _AP3_UART_H_