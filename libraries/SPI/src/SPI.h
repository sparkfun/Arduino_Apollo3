/*
 * SPI Master library for Arduino Zero.
 * Copyright (c) 2015 Arduino LLC
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _SPI_H_INCLUDED
#define _SPI_H_INCLUDED

#include <Arduino.h>
#include "ap3_iomaster.h"

// SPI_HAS_TRANSACTION means SPI has
//   - beginTransaction()
//   - endTransaction()
//   - usingInterrupt()
//   - SPISetting(clock, bitOrder, dataMode)
#define SPI_HAS_TRANSACTION 1

// // SPI_HAS_NOTUSINGINTERRUPT means that SPI has notUsingInterrupt() method
// #define SPI_HAS_NOTUSINGINTERRUPT 1

#define SPI_MODE0 AM_HAL_IOM_SPI_MODE_0
#define SPI_MODE1 AM_HAL_IOM_SPI_MODE_1
#define SPI_MODE2 AM_HAL_IOM_SPI_MODE_2
#define SPI_MODE3 AM_HAL_IOM_SPI_MODE_3

typedef enum
{
  ap3_spi_tx_only = 1,
  ap3_spi_rx_only = 2,
  ap3_spi_full_duplex = 3,
} ap3_spi_duplex_e;

class SPISettings
{
public:
  SPISettings(uint32_t clock, BitOrder bitOrder, uint8_t dataMode)
  {
    if (__builtin_constant_p(clock))
    {
      init_AlwaysInline(clock, bitOrder, dataMode);
    }
    else
    {
      init_MightInline(clock, bitOrder, dataMode);
    }
  }

  // Default speed set to 4MHz, SPI mode set to MODE 0 and Bit order set to MSB first.
  SPISettings()
  {
    init_AlwaysInline(4000000, MSBFIRST, SPI_MODE0);
  }

private:
public: // temporary
  void init_MightInline(uint32_t clock, BitOrder bitOrder, uint8_t dataMode)
  {
    init_AlwaysInline(clock, bitOrder, dataMode);
  }

  void init_AlwaysInline(uint32_t clock, BitOrder bitOrder, uint8_t dataMode) __attribute__((__always_inline__))
  {
    this->clockFreq = clock;
    this->bitOrder = bitOrder;
    this->dataMode = (am_hal_iom_spi_mode_e)dataMode;
  }

  uint32_t clockFreq;
  am_hal_iom_spi_mode_e dataMode;
  BitOrder bitOrder;

  friend class SPIClass;
};

class SPIClass : public IOMaster
{
public:
  SPIClass(uint8_t iom_instance);
  SPIClass(uint8_t iom_instance, ap3_spi_duplex_e duplex);

  byte transfer(uint8_t data);
  uint16_t transfer16(uint16_t data);
  void transfer(void *buf, size_t count);
  void transferOut(void *buf, size_t count);
  void transferIn(void *buf, size_t count);
  void transferOutIn(void* buf_out, void* buf_in, size_t count);

  // Transaction Functions
  void usingInterrupt(int interruptNumber);
  void notUsingInterrupt(int interruptNumber);
  void beginTransaction(SPISettings settings);
  void endTransaction(void);

  // SPI Configuration methods
  void attachInterrupt();
  void detachInterrupt();

  void begin();
  void end();

  void setBitOrder(BitOrder order);
  void setDataMode(uint8_t uc_mode);
  void setClockDivider(uint8_t uc_div);

private:
  void config(SPISettings settings);
  void _transfer(void *buf_out = NULL, void *buf_in = NULL, size_t count = 0);

  // Bit Order (need a better way to handle this)
  BitOrder _order;

  // Duplex
  ap3_spi_duplex_e _duplex;

  // Pads (for reference)
  ap3_gpio_pin_t _padSCLK;
  ap3_gpio_pin_t _padMOSI;
  ap3_gpio_pin_t _padMISO;

  // SERCOM *_p_sercom;
  // uint8_t _uc_pinMiso;
  // uint8_t _uc_pinMosi;
  // uint8_t _uc_pinSCK;

  // SercomSpiTXPad _padTx;
  // SercomRXPad _padRx;

  // bool initialized;
  // uint8_t interruptMode;
  // char interruptSave;
  // uint32_t interruptMask;
};

#if SPI_INTERFACES_COUNT > 0
extern SPIClass SPI;
#endif
#if SPI_INTERFACES_COUNT > 1
extern SPIClass SPI1;
#endif
#if SPI_INTERFACES_COUNT > 2
extern SPIClass SPI2;
#endif
#if SPI_INTERFACES_COUNT > 3
extern SPIClass SPI3;
#endif
#if SPI_INTERFACES_COUNT > 4
extern SPIClass SPI4;
#endif
#if SPI_INTERFACES_COUNT > 5
extern SPIClass SPI5;
#endif

// For compatibility with sketches designed for AVR @ 16 MHz
// New programs should use SPI.beginTransaction to set the SPI clock
#if F_CPU == 48000000
#define SPI_CLOCK_DIV2 6
#define SPI_CLOCK_DIV4 12
#define SPI_CLOCK_DIV8 24
#define SPI_CLOCK_DIV16 48
#define SPI_CLOCK_DIV32 96
#define SPI_CLOCK_DIV64 192
#define SPI_CLOCK_DIV128 255
#endif

#endif
