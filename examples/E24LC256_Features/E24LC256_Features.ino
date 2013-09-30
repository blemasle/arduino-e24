#include <E24LC256.h>
#include <Wire.h>

#define E24LC256_ADDR 0x50
E24LC256 e24lc = E24LC256(E24LC256_ADDR);

void setup()
{
	Wire.begin();
	Serial.begin(115200);
	randomSeed(123456789);
}

void loop()
{
	byte rand = (byte)random(0, 256);
	byte value;
	unsigned short addr = (unsigned short)random(65535);

	Serial.print("Sending ");
	Serial.print(rand, HEX);
	Serial.print(" to ");
	Serial.print(addr, HEX);
	Serial.println("...");

	e24lc.write(addr, rand);

	Serial.println("Data sent.");

	Serial.print("Reading ");
	Serial.print(addr, HEX);

	value = e24lc.read(addr);
	//value = rand;

	Serial.print(" : ");
	Serial.print(value, HEX);
	
	if(value == rand)
	{
		Serial.println(", SUCCESS !");
	}
	else
	{
		Serial.println(", FAILED !");
	}

	delay(3000);
}
