#include <SPI.h>

#include "Adns5050.h"

Adns5050::Adns5050 (const int ncsPin, const int resetPin)
{
	_ncsPin = ncsPin;
	_resetPin = resetPin;
#ifdef ADNS_50x0_DEBUG
	Serial.print("ADNS.NCS: "); Serial.println(_ncsPin);
	Serial.print("ADNS.NRESET: "); Serial.println(_resetPin);
#endif
}

bool Adns5050::begin (const unsigned long fsck)
{
	// Setup SPI pins
	SPI.begin();

	// Setup control pins
	if (_ncsPin > -1) {
		pinMode(_ncsPin, OUTPUT);
	}
	if (_resetPin > -1) {
		pinMode(_resetPin, OUTPUT);
		// Activate slave
		digitalWrite(_resetPin, HIGH);
	}

	// Setup SPI parameters
	if (fsck) _fSCK = fsck;
	else _fSCK = ADNS_50x0_fSCK;
	_spiSettings = SPISettings(_fSCK, MSBFIRST, SPI_MODE3);

	// Compute IO timings and delays
	_dSWR = (unsigned int)(ADNS_5050_tSWR - 8000000.0/_fSCK);
	_dSWW = (unsigned int)(ADNS_5050_tSWW - 16000000.0/_fSCK);

	// By now the sensor should have powered up, so we can...
	// Test Product_ID2 and Revision_ID
	if (read(Product_ID2) != ADNS_5050_Product_ID2) return false;
	if (read(Revision_ID) != ADNS_5050_Revision_ID) return false;
	if ((unsigned char)(~read(Inv_Rev_ID)) != ADNS_5050_Revision_ID) return false;

	return true;
}

unsigned char Adns5050::read (const ADNS_5050_reg address)
{
	// A READ following...
	switch (_lastOp)
	{
		// ...a READ
		case ADNS_50x0_READ:
			// Wait for tSRR
			delayMicroseconds(ADNS_5050_tSRR);
			break;
		// ...a WRITE
		case ADNS_50x0_WRITE:
			// Wait for dSWR - 1 byte
			delayMicroseconds(_dSWR);
			break;
	}

	SPI.beginTransaction(_spiSettings);
	if (_ncsPin > -1) digitalWrite(_ncsPin, LOW);

	unsigned char recv = SPI.transfer(address | ADNS_50x0_READ);
	if (recv != (address | ADNS_50x0_READ)) {
#ifdef ADNS_50x0_DEBUG
		Serial.println("ADNS.SDIO: IO error");
#endif
		SPI.endTransaction();
		return 0xff;
	} else {
		delayMicroseconds(ADNS_50x0_tSRAD);
#ifdef ADNS_50x0_DEBUG
		Serial.print("ADNS.SDIO << 0x");
		Serial.print(address, HEX);
		Serial.print(" >> 0x");
		Serial.println(recv, HEX);
#endif
	}

	pinMode(MOSI, INPUT);
	recv = SPI.transfer((unsigned char)0);
	pinMode(MOSI, OUTPUT);
#ifdef ADNS_50x0_DEBUG
	Serial.print("ADNS.SDIO >> 0x");
	Serial.println(recv, HEX);
#endif

	SPI.endTransaction();
	if (_ncsPin > -1) digitalWrite(_ncsPin, HIGH);

	_lastOp = ADNS_50x0_READ;
	return recv;
}

unsigned char Adns5050::write (const ADNS_5050_reg address, unsigned char value)
{
	switch (_lastOp)
	{
		case ADNS_50x0_READ:
			delayMicroseconds(ADNS_5050_tSRW);
			break;
		case ADNS_50x0_WRITE:
			delayMicroseconds(_dSWW);
			break;
	}

	digitalWrite(_ncsPin, LOW);
	SPI.beginTransaction(_spiSettings);

	unsigned char recv = SPI.transfer(address | ADNS_50x0_WRITE);
		if (recv != (address | ADNS_50x0_WRITE)) {
#ifdef ADNS_50x0_DEBUG
		Serial.println("ADNS.SDIO: IO error");
#endif
		SPI.endTransaction();
		return 0xff;
	}

	recv = SPI.transfer(value);

	digitalWrite(_ncsPin, HIGH);
	SPI.endTransaction();

	_lastOp = ADNS_50x0_WRITE;
	return recv;
}
