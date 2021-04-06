/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "Wire.h"

arduino::MbedI2C::MbedI2C(int sda, int scl) : _sda(sda), _scl(scl), usedTxBuffer(0), master(NULL) {
	#ifdef DEVICE_I2CSLAVE
    slave = NULL;
	#endif
}

void arduino::MbedI2C::begin() {
	if(!master){
		master = new mbed::I2C((PinName)_sda, (PinName)_scl);
		setClock(100000); //Default to 100kHz
	}
}

void arduino::MbedI2C::begin(uint8_t slaveAddr) {
#ifdef DEVICE_I2CSLAVE
	if(!slave){
		slave = new mbed::I2CSlave((PinName)_sda, (PinName)_scl);
	}
	slave->address(slaveAddr << 1);
	slave_th.start(mbed::callback(this, &arduino::MbedI2C::receiveThd));
#endif
}

void arduino::MbedI2C::end() {
	if (master != NULL) {
		delete master;
	}
#ifdef DEVICE_I2CSLAVE
	if (slave != NULL) {
		delete slave;
	}
#endif
}

void arduino::MbedI2C::setClock(uint32_t freq) {
	if (master != NULL) {
		master->frequency(freq);
	}
#ifdef DEVICE_I2CSLAVE
	if (slave != NULL) {
		slave->frequency(freq);
	}
#endif
}

void arduino::MbedI2C::beginTransmission(uint8_t address) {
	_address = address << 1;
	usedTxBuffer = 0;
}

uint8_t arduino::MbedI2C::endTransmission(void) {
	return endTransmission(true);
}

uint8_t arduino::MbedI2C::endTransmission(bool stopBit) {
	if (master->write(_address, (const char *) txBuffer, usedTxBuffer, !stopBit) == 0){
		return 0;
	}
	return 2;
}

uint8_t arduino::MbedI2C::requestFrom(uint8_t address, size_t len, bool stopBit) {
	char buf[256];
	int ret = master->read(address << 1, buf, len, !stopBit);
	if (ret != 0) {
		return 0;
	}
	for (size_t i=0; i<len; i++) {
		rxBuffer.store_char(buf[i]);
	}
	return len;
}

uint8_t arduino::MbedI2C::requestFrom(uint8_t address, size_t len) {
	return requestFrom(address, len, true);
}

size_t arduino::MbedI2C::write(uint8_t data) {
	if (usedTxBuffer == 256){
		return 0;
	}
	txBuffer[usedTxBuffer++] = data;
	return 1;
}

size_t arduino::MbedI2C::write(const uint8_t* data, int len) {
	if (usedTxBuffer + len > 256){
		len = 256 - usedTxBuffer;
	}
	memcpy(txBuffer + usedTxBuffer, data, len);
	usedTxBuffer += len;
	return len;
}

int arduino::MbedI2C::read() {
	if (rxBuffer.available()) {
		return rxBuffer.read_char();
	}
	return 0;
}

int arduino::MbedI2C::available() {
	return rxBuffer.available();
}

int arduino::MbedI2C::peek() {
	return rxBuffer.peek();
}

void arduino::MbedI2C::flush() {
}

void arduino::MbedI2C::receiveThd() {
#ifdef DEVICE_I2CSLAVE
	while (1) {
		int i = slave->receive();
		switch (i) {
			case mbed::I2CSlave::ReadAddressed:
				if (onRequestCb != NULL) {
					onRequestCb();
				}
				slave->write((const char *) txBuffer, usedTxBuffer);
				slave->stop();
				break;
			case mbed::I2CSlave::WriteGeneral:
			case mbed::I2CSlave::WriteAddressed:
				rxBuffer.clear();
				char buf[16];
				while (1) {
					int c = slave->read(buf, sizeof(buf));
					for (int i = 0; i < c; i++) {
						rxBuffer.store_char(uint8_t(buf[i]));
					}
					if (c <= sizeof(buf)) {
						break;
					}
				}
				if (rxBuffer.available() > 0 && onReceiveCb != NULL) {
					onReceiveCb(rxBuffer.available());
				}
				slave->stop();
				break;
		}
	}
#else
	MBED_ASSERT(0);
#endif
}

void arduino::MbedI2C::onReceive(voidFuncPtrParamInt cb) {
	onReceiveCb = cb;
}
void arduino::MbedI2C::onRequest(voidFuncPtr cb) {
	onRequestCb = cb;
}



#if VARIANT_WIRE_INTFCS > 0
arduino::MbedI2C Wire(VARIANT_Wire_SDA, VARIANT_Wire_SCL);
#endif
#if VARIANT_WIRE_INTFCS > 1
arduino::MbedI2C Wire1(VARIANT_Wire1_SDA, VARIANT_Wire1_SCL);
#endif
#if VARIANT_WIRE_INTFCS > 2
arduino::MbedI2C Wire2(VARIANT_Wire2_SDA, VARIANT_Wire2_SCL);
#endif
#if VARIANT_WIRE_INTFCS > 3
arduino::MbedI2C Wire3(VARIANT_Wire3_SDA, VARIANT_Wire3_SCL);
#endif
#if VARIANT_WIRE_INTFCS > 4
arduino::MbedI2C Wire4(VARIANT_Wire4_SDA, VARIANT_Wire4_SCL);
#endif
#if VARIANT_WIRE_INTFCS > 5
arduino::MbedI2C Wire5(VARIANT_Wire5_SDA, VARIANT_Wire5_SCL);
#endif
