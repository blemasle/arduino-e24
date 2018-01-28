#pragma once

#include <Arduino.h>
#include <Wire.h>

#define E24_DEFAULT_ADDR			0x50

enum class E24Size_t : uint8_t
{
	E24_8K = 0,
	E24_16K = 1,
	E24_32K = 2,
	E24_64K = 3,
	E24_128K = 4,
	E24_256K = 5,
	E24_512K = 6
};

class E24
{
private:
	uint8_t _deviceAddr;
	E24Size_t _size;

	int internalWrite(uint16_t addr, uint8_t* data, uint8_t length);
	int internalRead(uint16_t addr, uint8_t* data, uint16_t length);
	int burstWrite(uint16_t addr, uint8_t* data, uint16_t length);
	int burstRead(uint16_t addr, uint8_t* data, uint16_t length);
public:
	E24(E24Size_t size, uint8_t addr);
	~E24();
	
	uint8_t read();
	uint8_t read(uint16_t addr);
	int read(uint16_t addr, uint8_t* data, uint16_t length);

	void write(uint16_t addr, uint8_t data);
	int write(uint16_t addr, uint8_t* data, uint16_t length);

	template <typename T> int readBlock(uint16_t addr, const T& data)
	{
		return read(addr, (uint8_t*)&data, sizeof(T));
	}

	template <typename T> int writeBlock(uint16_t addr, const T& data)
	{
		return write(addr, (uint8_t*)&data, sizeof(T));
	}
};