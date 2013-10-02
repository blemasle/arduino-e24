#include "E24LC256_Features.h"

#include <E24LC256.h>
#include <Wire.h>

#define E24LC256_ADDR 0x50
E24LC256 e24lc = E24LC256(E24LC256_ADDR);

bool testReadValue(unsigned short addr, char assertValue)
{
	byte value = e24lc.read(addr);
	Serial.print("Reading ");
	Serial.print(addr);
	Serial.print(" : ");
	Serial.print((char)value);
	if(value == assertValue)
	{
		Serial.println(", SUCCESS !");
		return true;
	}
	else
	{
		Serial.println(", FAILED !");
		return false;
	}
}

Config defaultConfig() {
	Config c = {
		"Test",
		"1.24",
		0,
		16,
		48,
		3
	};

	for(int i = 0; i < 128; i++)
	{
		c.patches[i] = i;
	}

	return c;
}

void setup()
{
	Wire.begin();
	Serial.begin(115200);
	randomSeed(123456789);

	byte rand = (byte)random(0, 256);
	byte value;
	unsigned short addr = (unsigned short)random(0x7FFF);

	Serial.println("Testing single byte write/read...");
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
	
	Serial.println("=====================================");
	Serial.println("Testing arbitrary bytes write/read...");
	char str_data[]={"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nulla nunc quam, rutrum ut nisl vitae, porta laoreet purus. Nunc eget lorem consequat, mollis magna quis, interdum erat cras amet."};	
	char read_data[188];

	int len = strlen(str_data);
	Serial.print("Test string length : ");
	Serial.println(len);

	bool success = true;
	//char* str_read_data = (char*)malloc(len);
	addr = 136;

	Serial.println("Writing data one byte at a time...");
	Serial.println("Erasing data...");
	for(int i = 0; i < len; i++)
	{
		e24lc.write(addr + i, 0);
	}

	for(int i = 0; i < len; i++)
	{
		e24lc.write(addr + i, (byte)str_data[i]);
	}
	e24lc.read(addr, (byte*)read_data, len);
	read_data[187] = '\0';

	Serial.println("Original string value : ");
	Serial.println(str_data);
	Serial.println("Read string value : ");
	Serial.println(read_data);

	for(int i = 0; i < len; i += 10)
	{
		success &= testReadValue(addr + i, str_data[i]);
	}
	success &= testReadValue(addr + len - 1, str_data[len - 1]);
	success ? Serial.println("SUCCESS !") : Serial.println("FAILED !");
	
	Serial.println("=====================================");
	success = true;
	Serial.println("Writing data in sequential mode...");
	Serial.println("Erasing data...");
	for(int i = 0; i < len; i++)
	{
		e24lc.write(addr + i, 0);
	}

	e24lc.write(addr, (byte*)str_data, len);
	e24lc.read(addr, (byte*)read_data, len);
	read_data[187] = '\0';

	Serial.println("Original string value : ");
	Serial.println(str_data);
	Serial.println("Read string value : ");
	Serial.println(read_data);

	for(int i = 0; i < len; i += 10)
	{
		success &= testReadValue(addr + i, str_data[i]);
	}
	success &= testReadValue(addr + len - 1, str_data[len - 1]);
	success ? Serial.println("SUCCESS !") : Serial.println("FAILED !");

	Serial.println("=====================================");
	success = true;
	Serial.println("Writing data block mode...");
	Serial.println("Erasing data...");
	for(int i = 0; i < len; i++)
	{
		e24lc.write(addr + i, 0);
	}
	Config c = defaultConfig();
	e24lc.writeBlock(addr, c);
	Config c2;// = defaultConfig();
	e24lc.readBlock(addr, c2);
	Serial.println("==========Original value : ");
	Serial.print("lastUsedPatch : "); Serial.println(c.lastUsedPatch);
	Serial.print("lcdContrast : ");Serial.println(c.lcdContrast);
	Serial.print("rxChannel : ");Serial.println(c.rxChannel);
	Serial.print("txChannel : ");Serial.println(c.txChannel);
	Serial.print("version : ");Serial.println(c.version);
	Serial.print("seed : ");Serial.println(c.seed);
	
	Serial.println("==========Read value : ");
	Serial.print("lastUsedPatch : "); Serial.println(c2.lastUsedPatch);
	Serial.print("lcdContrast : ");Serial.println(c2.lcdContrast);
	Serial.print("rxChannel : ");Serial.println(c2.rxChannel);
	Serial.print("txChannel : ");Serial.println(c2.txChannel);
	Serial.print("version : ");Serial.println(c2.version);
	Serial.print("seed : ");Serial.println(c2.seed);

	success &= c.lastUsedPatch == c2.lastUsedPatch &&
		c.lcdContrast == c2.lcdContrast &&
		c.rxChannel == c2.rxChannel &&
		c.txChannel == c2.txChannel;

	len = strlen(c.seed);
	int len2 = strlen(c2.seed);

	success &= len == len2;
	if(success)
	{
		for(int i = 0; i < len; i++)
		{
			success &= c.seed[i] == c2.seed[i];
		}
	}

	len = strlen(c.version);
	len2 = strlen(c2.version);
	success &= len == len2;
	if(success)
	{
		for(int i = 0; i < len; i++)
		{
			success &= c.version[i] == c2.version[i];
		}
	}

	
	for(int i = 0; i < 128; i++)
	{
		success &= c.patches[i] == c2.patches[i];
	}

	success ? Serial.println("SUCCESS !") : Serial.println("FAILED !");
	
}

void loop()
{
	
}
