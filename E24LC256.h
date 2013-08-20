#ifndef _E24LC256_h
#define _E24LC256_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <Wire.h>

class E24LC256
{
private:
public:
	E24LC256(byte addr);
	~E24LC256();
};

#endif