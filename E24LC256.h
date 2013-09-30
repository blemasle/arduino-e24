#ifndef _E24LC256_h
#define _E24LC256_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <Wire.h>

#define E24LC256_ADDR 0x50
class E24LC256
{
private:
	void burstWrite(unsigned short addr, byte* value, int length);
	void burstRead(unsigned short addr, byte* value, int length);
public:
	E24LC256(byte addr);
	~E24LC256();
	
	byte read();
	byte read(unsigned short addr);
	void read(unsigned short page, byte* data);

	void write(unsigned short addr, byte value);
	void write(unsigned short page, byte* data);
};

#endif