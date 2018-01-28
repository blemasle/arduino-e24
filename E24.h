#pragma once

#include <Arduino.h>
#include <Wire.h>

#define E24_ADDR			0x50
#define WRITE_BUFFERSIZE	30
#define READ_BUFFERSIZE		30

#define E24_MAXADDRESS	(static_cast<uint16_t>(1 << static_cast<uint8_t>(_size)) * 1024) - 1
#define E24_PAGESIZE	(static_cast<uint8_t>(1 << ((static_cast<uint8_t>(_size) + 2) / 2)) * 8)

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
	int internalWrite(unsigned short addr, byte* data, byte length);
	int internalRead(unsigned short addr, byte* data, unsigned short length);
	int burstWrite(unsigned short addr, byte* data, unsigned short length);
	int burstRead(unsigned short addr, byte* data, unsigned short length);
	uint8_t _deviceAddr;
	E24Size_t _size;
public:
	E24(E24Size_t size, uint8_t addr);
	~E24();
	
	byte read();
	byte read(unsigned short addr);
	int read(unsigned short addr, byte* data, unsigned short length);

	void write(unsigned short addr, byte data);
	int write(unsigned short addr, byte* data, unsigned short length);

	template <typename T> int readBlock(unsigned short addr, const T& data)
	{
		return read(addr, (byte*)&data, sizeof(T));
	}

	template <typename T> int writeBlock(unsigned short addr, const T& data)
	{
		return write(addr, (byte*)&data, sizeof(T));
	}
};