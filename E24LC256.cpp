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
	Wire.endTransmission();
	
	//wait until the full page is being written
	delay(5);

	return length;
}

int internalRead(unsigned short addr, byte* data, byte length)
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

int E24LC256::burstWrite(unsigned short addr, byte* data, int length)
{
	if(addr + length - 1 > E24LC256_MAXADRESS) return -1;

	unsigned short offset = 0;
	byte pre = length < E24LC256_PAGESIZE ? length : addr % E24LC256_PAGESIZE;

	length -= pre;
	unsigned short buffers = length / WRITE_BUFFERSIZE;
	byte remain = length % WRITE_BUFFERSIZE;

	offset += internalWrite(addr + offset, data + offset, pre);

	//write n first full buffers
	for(int i = 0; i < buffers; i++)
	{
		offset += internalWrite(addr + offset, data + offset, WRITE_BUFFERSIZE);
	}

	//write additionnal content or initial content
	//if length < WRITE_BUFFERSIZE
	if(remain > 0) {
		offset += internalWrite(addr + offset, data + offset, remain);
	}

	return offset;
}

int E24LC256::burstRead(unsigned short addr, byte* data, int length)
{
	unsigned short buffers = length / READ_BUFFERSIZE;
	byte remain = length % READ_BUFFERSIZE;
	unsigned short offset = 0;
	byte val;

	//write n first full buffers
	for(int i = 0; i < buffers; i++)
	{
		offset += internalRead(addr + offset, data + offset, READ_BUFFERSIZE);
	}

	//write additionnal content or initial content
	//if length < READ_BUFFERSIZE
	if(remain > 0) {
		offset += internalRead(addr + offset, data + offset, remain);
	}

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

