#include "E24LC256.h"

byte _deviceAddr;

E24LC256::E24LC256(byte deviceAddr)
{
	_deviceAddr = deviceAddr;
}

E24LC256::~E24LC256() {}

void E24LC256::burstWrite(unsigned short addr, byte* value, int length)
{

}

void E24LC256::burstRead(unsigned short addr, byte* value, int length)
{

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

void E24LC256::read(unsigned short page, byte* data)
{

}

void E24LC256::write(unsigned short addr, byte value)
{
	Wire.beginTransmission(_deviceAddr);
	Wire.write(highByte(addr));
	Wire.write(lowByte(addr));
	Wire.write(value);
	Wire.endTransmission();

	//wait until the full page is being written
	delay(5);
}

void E24LC256::write(unsigned short page, byte* data)
{

}

