#pragma once

#include <Arduino.h>
#include <Wire.h>

#define E24LC_ADDR			0x50
#define E24LC_MAXADRESS		0x7FFF
#define E24LC_PAGESIZE		64
#define WRITE_BUFFERSIZE	30
#define READ_BUFFERSIZE		30

class E24LC
{
private:
	byte _deviceAddr;
	int internalWrite(unsigned short addr, byte* data, byte length);
	int internalRead(unsigned short addr, byte* data, unsigned short length);
	int burstWrite(unsigned short addr, byte* data, unsigned short length);
	int burstRead(unsigned short addr, byte* data, unsigned short length);
public:
	E24LC(byte addr);
	~E24LC();
	
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