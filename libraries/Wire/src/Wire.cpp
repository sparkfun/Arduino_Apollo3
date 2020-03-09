/*

Copyright (c) 2015 Arduino LLC. All rights reserved.
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

extern "C"
{
#include <string.h>
}

// #include <Arduino.h>
// #include <wiring_private.h>

#include "Wire.h"

typedef enum
{
	AP3_WIRE_SCL = 0x00,
	AP3_WIRE_SDA,
} ap3_iom_i2c_pad_type_e;

TwoWire::TwoWire(uint8_t iom_instance) : IOMaster(iom_instance)
{
	_transmissionBegun = false;
	_pullups = AM_HAL_GPIO_PIN_PULLUP_1_5K; //Default
	_clockSpeed = AM_HAL_IOM_100KHZ;
}

void TwoWire::begin(void)
{
	//Master Mode

	am_hal_gpio_pincfg_t pincfg = AP3_GPIO_DEFAULT_PINCFG;
	uint8_t funcsel = 0;

	ap3_err_t retval = AP3_OK;

	retval = ap3_iom_pad_funcsel(_instance, AP3_IOM_I2C_SCL, &_padSCL, &funcsel);
	if (retval != AP3_OK)
	{
		return /*retval*/;
	}
	pincfg.uFuncSel = funcsel; // set the proper function select option for this instance/pin/type combination
	pincfg.ePullup = _pullups;
	pincfg.eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_12MA;
	pincfg.eGPOutcfg = AM_HAL_GPIO_PIN_OUTCFG_OPENDRAIN;
	pincfg.uIOMnum = _instance;
	padMode(_padSCL, pincfg, &retval);
	if (retval != AP3_OK)
	{
		return /*ap3_return(retval)*/;
	}
	pincfg = AP3_GPIO_DEFAULT_PINCFG; // set back to default for use with next pin

	retval = ap3_iom_pad_funcsel(_instance, AP3_IOM_I2C_SDA, &_padSDA, &funcsel);
	if (retval != AP3_OK)
	{
		return /*retval*/;
	}
	pincfg.uFuncSel = funcsel;
	pincfg.ePullup = _pullups;
	pincfg.eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_12MA;
	pincfg.eGPOutcfg = AM_HAL_GPIO_PIN_OUTCFG_OPENDRAIN;
	pincfg.uIOMnum = _instance;
	padMode(_padSDA, pincfg, &retval);
	if (retval != AP3_OK)
	{
		return /*ap3_return(retval)*/;
	}
	pincfg = AP3_GPIO_DEFAULT_PINCFG; // set back to default for use with next pin

	memset((void *)&_config, 0x00, sizeof(am_hal_iom_config_t)); // Set the IOM configuration
	_config.eInterfaceMode = AM_HAL_IOM_I2C_MODE;
	_config.ui32ClockFreq = _clockSpeed;

	//Setup defaults that do not change
	iomTransfer.ui32InstrLen = 0; // Use only data phase
	iomTransfer.ui32Instr = 0;	//
	// iomTransfer.ui32NumBytes = ;         //
	iomTransfer.eDirection = AM_HAL_IOM_TX;
	iomTransfer.pui32TxBuffer = (uint32_t *)_linearBugger;
	iomTransfer.pui32RxBuffer = NULL;
	iomTransfer.ui8RepeatCount = 0;		// ?
	iomTransfer.ui8Priority = 1;		// ?
	iomTransfer.ui32PauseCondition = 0; // ?
	iomTransfer.ui32StatusSetClr = 0;   // ?

	initialize(); // Initialize the IOM
}

void TwoWire::begin(uint8_t address, bool enableGeneralCall)
{
	//Slave mode

	// ToDo:
}

void TwoWire::setClock(uint32_t baudrate)
{
	// ToDo: disable I2C while switching, if necessary
	_clockSpeed = baudrate;
	_config.ui32ClockFreq = _clockSpeed;
	initialize(); // Initialize the IOM
}

void TwoWire::setPullups(uint32_t pullupAmount)
{
	if (pullupAmount == 0)
		_pullups = AM_HAL_GPIO_PIN_PULLUP_NONE;
	if (pullupAmount > 0 && pullupAmount < 6)
		_pullups = AM_HAL_GPIO_PIN_PULLUP_1_5K;
	else if (pullupAmount >= 6 && pullupAmount < 12)
		_pullups = AM_HAL_GPIO_PIN_PULLUP_6K;
	else if (pullupAmount >= 12 && pullupAmount < 24)
		_pullups = AM_HAL_GPIO_PIN_PULLUP_12K;
	else if (pullupAmount >= 24)
		_pullups = AM_HAL_GPIO_PIN_PULLUP_24K;

	//Reinit I2C pins with this new pullup value
	begin();
}

void TwoWire::end()
{
	deinitialize(); //De init and power down this IOM
}

uint8_t TwoWire::requestFrom(uint8_t address, size_t quantity, bool stopBit)
{
	if (quantity == 0)
	{
		return 0;
	}
	size_t byteRead = 0;
	_rxBuffer.clear();

	am_hal_iom_transfer_t iomTransfer = {0};
	iomTransfer.uPeerInfo.ui32I2CDevAddr = address;
	iomTransfer.ui32InstrLen = 0;		 // 8-bit transfers
	iomTransfer.ui32Instr = 0;			 // Offset;
	iomTransfer.ui32NumBytes = quantity; // How many bytes to receive
	iomTransfer.eDirection = AM_HAL_IOM_RX;
	iomTransfer.pui32TxBuffer = NULL;
	iomTransfer.pui32RxBuffer = (uint32_t *)_linearBugger; // Link in the RX buffer
	iomTransfer.bContinue = stopBit ? false : true;
	iomTransfer.ui8RepeatCount = 0;		// ?
	iomTransfer.ui8Priority = 1;		// ?
	iomTransfer.ui32PauseCondition = 0; // ?
	iomTransfer.ui32StatusSetClr = 0;   // ?

	uint32_t retVal32 = am_hal_iom_blocking_transfer(_handle, &iomTransfer);
	if (retVal32 != 0)
	{
		// Serial.println("got an error on requestFrom");
		return retVal32;
	}

	// Copy the bytes into the rx buffer
	for (byteRead = 0; byteRead < quantity; byteRead++)
	{
		_rxBuffer.store_char(_linearBugger[byteRead]); // Read data and send the ACK
	}

	return byteRead;
}

void TwoWire::beginTransmission(uint8_t address)
{
	// save address of target and clear buffer
	_transmissionAddress = address;
	_txBuffer.clear();

	_transmissionBegun = true;
}

// Errors:
//  0 : Success
//  1 : Data too long
//  2 : NACK on transmit of address
//  3 : NACK on transmit of data
//  4 : Other error
uint8_t TwoWire::endTransmission(bool stopBit)
{
	_transmissionBegun = false;

	iomTransfer.uPeerInfo.ui32I2CDevAddr = _transmissionAddress;
	iomTransfer.bContinue = (stopBit ? false : true); // whether or not to hold onto the bus after this transfer

	// Copy the bytes from the TX Buffer into the linear buffer
	size_t count = 0;
	if (_txBuffer.available() > AP3_WIRE_LINEAR_BUFFER_LEN)
	{
		return 1; // data too long
	}
	while (_txBuffer.available())
	{
		*(_linearBugger + count++) = _txBuffer.read_char();
	}

	iomTransfer.ui32NumBytes = count; // Set the length

	// ToDo: better error reporting
	uint32_t retVal32 = am_hal_iom_blocking_transfer(_handle, &iomTransfer);
	switch (retVal32)
	{
	case AM_HAL_STATUS_SUCCESS:
		return 0;
		break;

		// return 1; // data too long
		// return 2; // NAK on address
		// return 3; // NAK on data

	case AM_HAL_STATUS_INVALID_OPERATION:
	case AM_HAL_STATUS_INVALID_ARG:
	case AM_HAL_STATUS_INVALID_HANDLE:
	default:
		return 4;
		break; // other error
	}
}

size_t TwoWire::write(uint8_t ucData)
{
	// No writing, without begun transmission or a full buffer
	if (!_transmissionBegun || _txBuffer.isFull())
	{
		return 0;
	}
	_txBuffer.store_char(ucData);
	return 1;
}

size_t TwoWire::write(const uint8_t *data, size_t quantity)
{
	for (size_t i = 0; i < quantity; ++i)
	{ //Try to store all data
		if (!write(data[i]))
		{ //Return the number of data stored, when the buffer is full (if write return 0)
			return i;
		}
	}
	return quantity; //All data stored
}

int TwoWire::available(void)
{
	return _rxBuffer.available();
}

int TwoWire::read(void)
{
	return _rxBuffer.read_char();
}

int TwoWire::peek(void)
{
	return _rxBuffer.peek();
}

void TwoWire::flush(void)
{
	// Do nothing, use endTransmission(..) to force
	// data transfer.
}

void TwoWire::onReceive(void (*function)(int))
{
	_onReceiveCallback = function;
}

void TwoWire::onRequest(void (*function)(void))
{
	_onRequestCallback = function;
}

void TwoWire::onService(void)
{
	// ToDo:

	// if ( sercom->isSlaveWIRE() )
	// {
	// 	if(sercom->isStopDetectedWIRE() ||
	// 		(sercom->isAddressMatch() && sercom->isRestartDetectedWIRE() && !sercom->isMasterReadOperationWIRE())) //Stop or Restart detected
	// 	{
	// 		sercom->prepareAckBitWIRE();
	// 		sercom->prepareCommandBitsWire(0x03);

	// 		//Calling onReceiveCallback, if exists
	// 		if(onReceiveCallback)
	// 		{
	// 		onReceiveCallback(available());
	// 		}

	// 		rxBuffer.clear();
	// 	}
	// 	else if(sercom->isAddressMatch())  //Address Match
	// 	{
	// 		sercom->prepareAckBitWIRE();
	// 		sercom->prepareCommandBitsWire(0x03);

	// 		if(sercom->isMasterReadOperationWIRE()) //Is a request ?
	// 		{
	// 		txBuffer.clear();

	// 		transmissionBegun = true;

	// 		//Calling onRequestCallback, if exists
	// 		if(onRequestCallback)
	// 		{
	// 			onRequestCallback();
	// 		}
	// 		}
	// 	}
	// 	else if(sercom->isDataReadyWIRE())
	// 	{
	// 		if (sercom->isMasterReadOperationWIRE())
	// 		{
	// 		uint8_t c = 0xff;

	// 		if( txBuffer.available() ) {
	// 			c = txBuffer.read_char();
	// 		}

	// 		transmissionBegun = sercom->sendDataSlaveWIRE(c);
	// 		} else { //Received data
	// 		if (rxBuffer.isFull()) {
	// 			sercom->prepareNackBitWIRE();
	// 		} else {
	// 			//Store data
	// 			rxBuffer.store_char(sercom->readDataWIRE());

	// 			sercom->prepareAckBitWIRE();
	// 		}

	// 		sercom->prepareCommandBitsWire(0x03);
	// 		}
	// 	}
	// }
}

// In variant.h define WIRE_INTERFACES_COUNT as well as the io master instance to use for each AP3_Wire_IOM and AP3_WireN_IOM [1 <= N <= 5]

#if WIRE_INTERFACES_COUNT > 0
TwoWire Wire(AP3_Wire_IOM);

//   void WIRE_IT_HANDLER(void) { // example of how you might implement directable callbacks. #define WIRE_IT_HANDLER your_handler in variant.h
//     Wire.onService();
//   }
#endif

#if WIRE_INTERFACES_COUNT > 1
TwoWire Wire1(AP3_Wire1_IOM);
#endif

#if WIRE_INTERFACES_COUNT > 2
TwoWire Wire2(AP3_Wire1_IOM);
#endif

#if WIRE_INTERFACES_COUNT > 3
TwoWire Wire3(AP3_Wire1_IOM);
#endif

#if WIRE_INTERFACES_COUNT > 4
TwoWire Wire4(AP3_Wire1_IOM);
#endif

#if WIRE_INTERFACES_COUNT > 5
TwoWire Wire5(AP3_Wire1_IOM);
#endif