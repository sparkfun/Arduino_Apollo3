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

#include "SPI.h"
#include <Arduino.h>
// #include <wiring_private.h>
#include <assert.h>

// #define SPI_IMODE_NONE   0
// #define SPI_IMODE_EXTINT 1
// #define SPI_IMODE_GLOBAL 2

const SPISettings DEFAULT_SPI_SETTINGS = SPISettings();

SPIClass::SPIClass(uint8_t iom_instance) : IOMaster(iom_instance)
{
  _duplex = ap3_spi_full_duplex;
}

SPIClass::SPIClass(uint8_t iom_instance, ap3_spi_duplex_e duplex) : IOMaster(iom_instance)
{
  _duplex = duplex;
}

void SPIClass::begin()
{
  ap3_err_t retval = AP3_OK;
  am_hal_gpio_pincfg_t pincfg = AP3_GPIO_DEFAULT_PINCFG;
  uint8_t funcsel = 0;

  // init();

  // Set up pins
  // clock
  retval = ap3_iom_pad_funcsel(_instance, AP3_IOM_SPI_SCLK, &_padSCLK, &funcsel);
  if (retval != AP3_OK)
  {
    return /*retval*/;
  }
  pincfg.uFuncSel = funcsel;
  pincfg.eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_12MA;
  pincfg.eGPOutcfg = AM_HAL_GPIO_PIN_OUTCFG_PUSHPULL;
  pincfg.uIOMnum = _instance;
  padMode(_padSCLK, pincfg, &retval);
  if (retval != AP3_OK)
  {
    return /*ap3_return(retval)*/;
  }
  pincfg = AP3_GPIO_DEFAULT_PINCFG; // set back to default for use with next pin

  // mosi
  if (_duplex & ap3_spi_tx_only)
  {
    retval = ap3_iom_pad_funcsel(_instance, AP3_IOM_SPI_MOSI, &_padMOSI, &funcsel);
    if (retval != AP3_OK)
    {
      return /*retval*/;
    }
    pincfg.uFuncSel = funcsel;
    pincfg.eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_12MA;
    pincfg.eGPOutcfg = AM_HAL_GPIO_PIN_OUTCFG_PUSHPULL;
    pincfg.uIOMnum = _instance;
    padMode(_padMOSI, pincfg, &retval);
    if (retval != AP3_OK)
    {
      return /*ap3_return(retval)*/;
    }
    pincfg = AP3_GPIO_DEFAULT_PINCFG; // set back to default for use with next pin
  }

  // miso
  if (_duplex & ap3_spi_rx_only)
  {
    retval = ap3_iom_pad_funcsel(_instance, AP3_IOM_SPI_MISO, &_padMISO, &funcsel);
    if (retval != AP3_OK)
    {
      return /*retval*/;
    }
    pincfg.uFuncSel = funcsel;
    pincfg.eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_12MA;
    pincfg.eGPOutcfg = AM_HAL_GPIO_PIN_OUTCFG_PUSHPULL;
    pincfg.uIOMnum = _instance;
    padMode(_padMISO, pincfg, &retval);
    if (retval != AP3_OK)
    {
      return /*ap3_return(retval)*/;
    }
    pincfg = AP3_GPIO_DEFAULT_PINCFG; // set back to default for use with next pin
  }

  config(DEFAULT_SPI_SETTINGS);
}

void SPIClass::config(SPISettings settings)
{
  memset((void *)&_config, 0x00, sizeof(am_hal_iom_config_t)); // Set the IOM configuration
  _config.eInterfaceMode = AM_HAL_IOM_SPI_MODE;
  _config.ui32ClockFreq = settings.clockFreq;
  _config.eSpiMode = settings.dataMode;
  _bitOrder = settings.bitOrder;

  initialize(); // Initialize the IOM
}

void SPIClass::end()
{
  deinitialize();
}

#ifndef interruptsStatus
#define interruptsStatus() __interruptsStatus()
static inline unsigned char __interruptsStatus(void) __attribute__((always_inline, unused));
static inline unsigned char __interruptsStatus(void)
{
  // // See http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0497a/CHDBIBGJ.html
  // return (__get_PRIMASK() ? 0 : 1);
}
#endif

void SPIClass::usingInterrupt(int interruptNumber)
{
  // if ((interruptNumber == NOT_AN_INTERRUPT) || (interruptNumber == EXTERNAL_INT_NMI))
  //   return;

  // uint8_t irestore = interruptsStatus();
  // noInterrupts();

  // if (interruptNumber >= EXTERNAL_NUM_INTERRUPTS)
  //   interruptMode = SPI_IMODE_GLOBAL;
  // else
  // {
  //   interruptMode |= SPI_IMODE_EXTINT;
  //   interruptMask |= (1 << g_APinDescription[interruptNumber].ulExtInt);
  // }

  // if (irestore)
  //   interrupts();
}

void SPIClass::notUsingInterrupt(int interruptNumber)
{
  // if ((interruptNumber == NOT_AN_INTERRUPT) || (interruptNumber == EXTERNAL_INT_NMI))
  //   return;

  // if (interruptMode & SPI_IMODE_GLOBAL)
  //   return; // can't go back, as there is no reference count

  // uint8_t irestore = interruptsStatus();
  // noInterrupts();

  // interruptMask &= ~(1 << g_APinDescription[interruptNumber].ulExtInt);

  // if (interruptMask == 0)
  //   interruptMode = SPI_IMODE_NONE;

  // if (irestore)
  //   interrupts();
}

void SPIClass::beginTransaction(SPISettings settings)
{
  // if (interruptMode != SPI_IMODE_NONE)
  // {
  //   if (interruptMode & SPI_IMODE_GLOBAL)
  //   {
  //     interruptSave = interruptsStatus();
  //     noInterrupts();
  //   }
  //   else if (interruptMode & SPI_IMODE_EXTINT)
  //     EIC->INTENCLR.reg = EIC_INTENCLR_EXTINT(interruptMask);
  // }

  config(settings);
}

void SPIClass::endTransaction(void)
{
  // if (interruptMode != SPI_IMODE_NONE)
  // {
  //   if (interruptMode & SPI_IMODE_GLOBAL)
  //   {
  //     if (interruptSave)
  //       interrupts();
  //   }
  //   else if (interruptMode & SPI_IMODE_EXTINT)
  //     EIC->INTENSET.reg = EIC_INTENSET_EXTINT(interruptMask);
  // }
}

void SPIClass::setBitOrder(BitOrder order)
{
  _bitOrder = order;

  //MSB/LSB is not exposed in the HAL v2.2 in a nice way
  //This is a hack to set the SPILSB bit of the MSPICFG register
  //Note that calling initialize() will overwrite the LSB bit
  if (order == LSBFIRST)
  {
    AM_REGVAL(0x50004300 + ((uint32_t)0x1000 * _instance)) |= (1 << 23);
  }
}

void SPIClass::setDataMode(uint8_t mode)
{
  _config.eSpiMode = (am_hal_iom_spi_mode_e)mode;
  initialize();
}

void SPIClass::setClockDivider(uint8_t div)
{
  _config.ui32ClockFreq = F_CPU / div;
  initialize();
}

byte SPIClass::transfer(uint8_t data)
{
  _transfer(&data, NULL, 1);
}

uint16_t SPIClass::transfer16(uint16_t data)
{
  union {
    uint16_t val;
    struct
    {
      uint8_t lsb;
      uint8_t msb;
    };
  } t;

  t.val = data;

  if (_bitOrder == LSBFIRST)
  {
    t.lsb = transfer(t.lsb);
    t.msb = transfer(t.msb);
  }
  else
  {
    t.msb = transfer(t.msb);
    t.lsb = transfer(t.lsb);
  }

  return t.val;
}

void SPIClass::transfer(void *buf, size_t count)
{
  _transfer(buf, buf, count); // todo: not sure how the HAL will handle sending and receiving from the same buffer...
}

void SPIClass::transferOut(void *buf, size_t count)
{
  _transfer(buf, NULL, count);
}

void SPIClass::transferIn(void *buf, size_t count)
{
  _transfer(NULL, buf, count);
}

void SPIClass::_transfer(void *buf_out, void *buf_in, size_t count)
{
  am_hal_iom_transfer_t iomTransfer = {0};
  // iomTransfer.uPeerInfo.ui32SpiChipSelect = cs_pad;
  iomTransfer.ui32InstrLen = 0;     // No instructions
  iomTransfer.ui32Instr = 0;        // No instructions
  iomTransfer.ui32NumBytes = count; // How many bytes to transfer
  // iomTransfer.eDirection = AM_HAL_IOM_TX; // AM_HAL_IOM_FULLDUPLEX - Note: Ambiq SDK says that FULLDUPLEX is not yet supported // todo:
  iomTransfer.pui32TxBuffer = (uint32_t *)buf_out; // todo: does this have the proper lifetime?
  iomTransfer.pui32RxBuffer = (uint32_t *)buf_in;
  iomTransfer.bContinue = false;
  iomTransfer.ui8RepeatCount = 0;     // ?
  iomTransfer.ui8Priority = 1;        // ?
  iomTransfer.ui32PauseCondition = 0; // ?
  iomTransfer.ui32StatusSetClr = 0;   // ?

  // Determine direction
  if ((buf_out != NULL) && (buf_in != NULL))
  {
    iomTransfer.eDirection = AM_HAL_IOM_TX; // AM_HAL_IOM_FULLDUPLEX - Note: Ambiq SDK says that FULLDUPLEX is not yet supported // todo:
  }
  else if (buf_out != NULL)
  {
    iomTransfer.eDirection = AM_HAL_IOM_TX;
  }
  else if (buf_in != NULL)
  {
    iomTransfer.eDirection = AM_HAL_IOM_RX;
  }

  uint32_t retVal32 = am_hal_iom_blocking_transfer(_handle, &iomTransfer);
  if (retVal32 != 0)
  {
    // Serial.printf("got an error on _transfer: %d\n", retVal32);
    return /*retVal32*/;
  }
}

void SPIClass::attachInterrupt()
{
  // Should be enableInterrupt()
}

void SPIClass::detachInterrupt()
{
  // Should be disableInterrupt()
}

#if SPI_INTERFACES_COUNT > 0
SPIClass SPI(AP3_SPI_IOM, AP3_SPI_DUP);
#endif
#if SPI_INTERFACES_COUNT > 1
SPIClass SPI1(AP3_SPI1_IOM, AP3_SPI1_DUP);
#endif
#if SPI_INTERFACES_COUNT > 2
SPIClass SPI2(AP3_SPI2_IOM, AP3_SPI2_DUP);
#endif
#if SPI_INTERFACES_COUNT > 3
SPIClass SPI3(AP3_SPI3_IOM, AP3_SPI3_DUP);
#endif
#if SPI_INTERFACES_COUNT > 4
SPIClass SPI4(AP3_SPI4_IOM, AP3_SPI4_DUP);
#endif
#if SPI_INTERFACES_COUNT > 5
SPIClass SPI5(AP3_SPI5_IOM, AP3_SPI5_DUP);
#endif
