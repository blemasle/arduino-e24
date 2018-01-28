#include "E24.h"


#define E24_MAX_ADDRESS			(static_cast<uint16_t>(1 << static_cast<uint8_t>(_size)) * 1024) - 1
#define E24_PAGE_SIZE			(static_cast<uint8_t>(1 << ((static_cast<uint8_t>(_size) + 2) / 2)) * 8)
#define E24_PAGE_WRITE_CYCLE	5

#define WRITE_BUFFER_SIZE		E24_PAGE_SIZE
#define READ_BUFFER_SIZE		E24_PAGE_SIZE

E24::E24(E24Size_t size, uint8_t deviceAddr)
{
	_deviceAddr = deviceAddr;
}

E24::~E24() {}

int E24::internalWrite(uint16_t addr, uint8_t* data, uint8_t length)
{
	Wire.beginTransmission(_deviceAddr);
	Wire.write(highByte(addr));
	Wire.write(lowByte(addr));

	Wire.write(data, length);
	uint8_t r = Wire.endTransmission();

	//wait until the full page is being written
	delay(E24_PAGE_WRITE_CYCLE);

	return length;
}

int E24::internalRead(uint16_t addr, uint8_t* data, uint16_t length)
{
	uint8_t offset = 0;

	Wire.beginTransmission(_deviceAddr);
	Wire.write(highByte(addr));
	Wire.write(lowByte(addr));
	Wire.endTransmission();

	Wire.requestFrom(_deviceAddr, length);
	while(Wire.available()) data[offset++] = Wire.read();

	return offset;
}

int E24::burstWrite(uint16_t addr, uint8_t* data, uint16_t length)
{
	if(addr + length - 1 > E24_MAX_ADDRESS) return -1;

	uint8_t pageSize = E24_PAGE_SIZE;
	uint16_t offset = 0;
	uint8_t bSize = 0;

	do {
		//compute the next buffer size using nextPageAddress - addr
		bSize = ((addr + pageSize) & ~(pageSize - 1)) - addr;
		//avoid to overflow content length & max write buffer size
		bSize = min(min(pageSize, bSize), length);

		length -= internalWrite(addr, data + offset, bSize);
		addr += bSize;
		offset += bSize;

	}while(length > 0);

	return offset;
}

int E24::burstRead(uint16_t addr, uint8_t* data, uint16_t length)
{
	uint8_t pageSize = E24_PAGE_SIZE;
	uint16_t offset = 0;
	uint8_t bSize = 0;

	do {
		//avoid to overflow max read buffer size
		bSize = min(pageSize, length);

		length -= internalRead(addr, data + offset, bSize);
		addr += bSize;
		offset += bSize;

	}while(length > 0);

	return offset;
}

uint8_t E24::read()
{
	Wire.beginTransmission(_deviceAddr);
	Wire.endTransmission();
	Wire.requestFrom(_deviceAddr, (uint8_t)1);
	
	return Wire.read();
}

uint8_t E24::read(uint16_t addr)
{
	Wire.beginTransmission(_deviceAddr);
	Wire.write(highByte(addr));
	Wire.write(lowByte(addr));
	Wire.endTransmission();
	Wire.requestFrom(_deviceAddr, (uint8_t)1);

	return Wire.read();
}

int E24::read(uint16_t addr, uint8_t* data, uint16_t length)
{
	return burstRead(addr, data, length);
}

void E24::write(uint16_t addr, uint8_t data)
{
	Wire.beginTransmission(_deviceAddr);
	Wire.write(highByte(addr));
	Wire.write(lowByte(addr));
	Wire.write(data);
	Wire.endTransmission();

	//wait until the full page is being written
	delay(E24_PAGE_WRITE_CYCLE);
}

int E24::write(uint16_t addr, uint8_t* data, uint16_t length)
{
	return burstWrite(addr, data, length);
}

