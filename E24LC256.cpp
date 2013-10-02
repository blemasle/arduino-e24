#include "E24LC256.h"

byte _deviceAddr;

E24LC256::E24LC256(byte deviceAddr)
{
	_deviceAddr = deviceAddr;
}

E24LC256::~E24LC256() {}

int E24LC256::burstWrite(unsigned short addr, byte* data, int length)
{
	unsigned short offset = 0;
	byte pre = addr % E24LC256_PAGESIZE;

	length -= pre;
	unsigned short buffers = length / WRITE_BUFFERSIZE;
	byte remain = length % WRITE_BUFFERSIZE;

	Wire.beginTransmission(_deviceAddr);
	Wire.write(highByte(addr + offset));
	Wire.write(lowByte(addr + offset));

	Wire.write(data, pre);
	Wire.endTransmission();
	offset += pre;

	//wait until the full page is being written
	delay(5);

	//write n first full buffers
	for(int i = 0; i < buffers; i++)
	{
		Wire.beginTransmission(_deviceAddr);
		Wire.write(highByte(addr + offset));
		Wire.write(lowByte(addr + offset));

		Wire.write(data + offset, WRITE_BUFFERSIZE);
		Wire.endTransmission();
		offset += WRITE_BUFFERSIZE;

		//wait until the full page is being written
		delay(5);
	}

	//write additionnal content or initial content
	//if length < WRITE_BUFFERSIZE
	if(remain > 0) {
		Wire.beginTransmission(_deviceAddr);
		Wire.write(highByte(addr + offset));
		Wire.write(lowByte(addr + offset));

		Wire.write(data + offset, remain);
		Wire.endTransmission();
		offset += remain;

		//wait until the full page is being written
		delay(5);
	}

	return offset;
}

void E24LC256::burstRead(unsigned short addr, byte* data, int length)
{
	unsigned short buffers = length / READ_BUFFERSIZE;
	byte remain = length % READ_BUFFERSIZE;
	unsigned short offset = 0;
	byte val;

	//write n first full buffers
	for(int i = 0; i < buffers; i++)
	{
		Serial.print("Request ");
		Serial.print(READ_BUFFERSIZE);
		Serial.print(" from ");
		Serial.println(addr + offset);

		Wire.beginTransmission(_deviceAddr);
		Wire.write(highByte(addr + offset));
		Wire.write(lowByte(addr + offset));
		Wire.endTransmission();

		Wire.requestFrom(_deviceAddr, (byte)READ_BUFFERSIZE);
		while(Wire.available()) { 
			val = Wire.read();
			data[offset++] = val; 
			Serial.println((char)val);
		}
	}

	//write additionnal content or initial content
	//if length < READ_BUFFERSIZE
	if(remain > 0) {

		Serial.print("Request ");
		Serial.print(remain);
		Serial.print(" from ");
		Serial.println(addr + offset);

		Wire.beginTransmission(_deviceAddr);
		Wire.write(highByte(addr + offset));
		Wire.write(lowByte(addr + offset));
		Wire.endTransmission();

		Wire.requestFrom(_deviceAddr, remain);
		while(Wire.available()) { 
			val = Wire.read();
			data[offset++] = val; 
			Serial.println((char)val);
		}
	}
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

void E24LC256::read(unsigned short addr, byte* data, unsigned short length)
{
	burstRead(addr, data, length);
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
	if(addr + length > E24LC256_MAXADRESS) return -1;
	return burstWrite(addr, data, length);
}


