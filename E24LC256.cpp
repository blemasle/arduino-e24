#include "E24LC256.h"

byte _deviceAddr;

E24LC256::E24LC256(byte deviceAddr)
{
	_deviceAddr = deviceAddr;
}

E24LC256::~E24LC256() {}

int internalWrite(unsigned short addr, byte* data, byte length)
{
	Wire.beginTransmission(_deviceAddr);
	Wire.write(highByte(addr));
	Wire.write(lowByte(addr));

	Wire.write(data, length);
	byte r = Wire.endTransmission();

	//wait until the full page is being written
	delay(5);

	return length;
}

int internalRead(unsigned short addr, byte* data, unsigned short length)
{
	byte offset = 0;

	Wire.beginTransmission(_deviceAddr);
	Wire.write(highByte(addr));
	Wire.write(lowByte(addr));
	Wire.endTransmission();

	Wire.requestFrom(_deviceAddr, length);
	while(Wire.available()) data[offset++] = Wire.read();

	return offset;
}

int E24LC256::burstWrite(unsigned short addr, byte* data, unsigned short length)
{
	if(addr + length - 1 > E24LC256_MAXADRESS) return -1;

	unsigned short offset = 0;
	byte bSize = 0;

	do {
		//compute the next buffer size using nextPageAddress - addr
		bSize = ((addr + E24LC256_PAGESIZE) & ~(E24LC256_PAGESIZE - 1)) - addr;
		//avoid to overflow content length & max write buffer size
		bSize = min(min(WRITE_BUFFERSIZE, bSize), length);

		length -= internalWrite(addr, data + offset, bSize);
		addr += bSize;
		offset += bSize;

	}while(length > 0);

	return offset;
}

int E24LC256::burstRead(unsigned short addr, byte* data, unsigned short length)
{
	unsigned short offset = 0;
	byte bSize = 0;

	do {
		//avoid to overflow max read buffer size
		bSize = min(READ_BUFFERSIZE, length);

		length -= internalRead(addr, data + offset, bSize);
		addr += bSize;
		offset += bSize;

	}while(length > 0);

	return offset;
}

byte E24LC256::read()
{
	Wire.beginTransmission(_deviceAddr);
	Wire.endTransmission();
	Wire.requestFrom(_deviceAddr, (byte)1);
	
	return Wire.read();
}

byte E24LC256::read(unsigned short addr)
{
	Wire.beginTransmission(_deviceAddr);
	Wire.write(highByte(addr));
	Wire.write(lowByte(addr));
	Wire.endTransmission();
	Wire.requestFrom(_deviceAddr, (byte)1);

	return Wire.read();
}

int E24LC256::read(unsigned short addr, byte* data, unsigned short length)
{
	return burstRead(addr, data, length);
}

void E24LC256::write(unsigned short addr, byte data)
{
	Wire.beginTransmission(_deviceAddr);
	Wire.write(highByte(addr));
	Wire.write(lowByte(addr));
	Wire.write(data);
	Wire.endTransmission();

	//wait until the full page is being written
	delay(5);
}

int E24LC256::write(unsigned short addr, byte* data, unsigned short length)
{
	return burstWrite(addr, data, length);
}

