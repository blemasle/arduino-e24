#include "E24_Features.h"

#include <E24.h>
#include <Wire.h>

#define E24_ADDR 0x50
E24 e24 = E24(E24_ADDR);

bool testChar(unsigned short addr, char assertValue)
{
	byte value = e24.read(addr);
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
		Serial.print(", FAILED !");
		Serial.print(" EXPECTED : ");
		Serial.println(assertValue);
		return false;
	}
}

bool testSingleByte()
{
	byte rand = (byte)random(0, 256);
	byte value;
	unsigned short addr = (unsigned short)random(0x7FFF);

	Serial.println("=====================================");
	Serial.println("Testing single byte write/read...");
	Serial.print("Sending ");
	Serial.print(rand, HEX);
	Serial.print(" to ");
	Serial.print(addr, HEX);
	Serial.println("...");
	Serial.println("=====================================");

	e24.write(addr, rand);

	Serial.println("Data sent.");

	Serial.print("Reading ");
	Serial.print(addr, HEX);

	value = e24.read(addr);
	//value = rand;

	Serial.print(" : ");
	Serial.print(value, HEX);
	
	if(value == rand)
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

bool testString()
{
	char str_data[]={"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nulla nunc quam, rutrum ut nisl vitae, porta laoreet purus. Nunc eget lorem consequat, mollis magna quis, interdum erat cras amet."};	
	char read_data[188];
	int len = strlen(str_data);
	Serial.println("=====================================");
	Serial.println("Testing arbitrary bytes write/read...");
	Serial.print("Test string length : ");
	Serial.println(len);

	bool success = true;
	unsigned short addr = 129;
//	unsigned short addr = (unsigned short)random(0x7F00);

	Serial.println("Writing data one byte at a time...");
	Serial.println("Erasing data...");
	for(int i = 0; i < len; i++)
	{
		e24.write(addr + i, 0);
	}
	Serial.println("Data erased.");
	Serial.println("Writing data.");
	for(int i = 0; i < len; i++)
	{
		e24.write(addr + i, (byte)str_data[i]);
	}
	
	Serial.println("Data written.");
	Serial.println("Reading written data.");
	e24.read(addr, (byte*)read_data, len);
	read_data[187] = '\0';

	Serial.println("Data read.");
	Serial.println("Original string value : ");
	Serial.println("=====================================");
	Serial.println(str_data);
	Serial.println("=====================================");
	Serial.println("Read string value : ");
	Serial.println("=====================================");
	Serial.println(read_data);

	for(int i = 0; i < len; i += 10)
	{
		success &= testChar(addr + i, str_data[i]);
	}
	success &= testChar(addr + len - 1, str_data[len - 1]);
	success ? Serial.println("SUCCESS !") : Serial.println("FAILED !");

	Serial.println("=====================================");
	Serial.println("Writing data in sequential mode...");
	Serial.println("Erasing data...");
	for(int i = 0; i < len; i++)
	{
		e24.write(addr + i, 0);
	}

	Serial.println("=====================================");
	Serial.println("Data erased.");
	Serial.println("Writing data.");
	e24.write(addr, (byte*)str_data, len);
	Serial.println("Data written.");
	Serial.println("Reading written data.");
	e24.read(addr, (byte*)read_data, len);
	Serial.println("Data read.");
	read_data[187] = '\0';
	Serial.println("Original string value : ");
	Serial.println("=====================================");
	Serial.println(str_data);
	Serial.println("=====================================");
	Serial.println("Read string value : ");
	Serial.println("=====================================");
	Serial.println(read_data);

	for(int i = 0; i < len; i += 10)
	{
		success &= testChar(addr + i, str_data[i]);
	}
	success &= testChar(addr + len - 1, str_data[len - 1]);
	success ? Serial.println("SUCCESS !") : Serial.println("FAILED !");
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

bool testBlock()
{
	bool success = true;
	int len = sizeof(Config); 
	int len2;
	unsigned short addr =136;
	Config c = defaultConfig();
	Config c2;

	Serial.println("=====================================");
	Serial.println("Writing data block mode...");
	Serial.println("Erasing data...");
	Serial.println("=====================================");
	for(int i = 0; i < len; i++)
	{
		e24.write(addr + i, 0);
	}
	Serial.println("Data erased.");
	Serial.println("Writing data.");
	e24.writeBlock(addr, c);
	Serial.println("Data written.");
	Serial.println("Reading written data.");
	Serial.println("=====================================");
	e24.readBlock(addr, c2);
	Serial.println("Original value : ");
	Serial.println("=====================================");
	Serial.print("lastUsedPatch : "); Serial.println(c.lastUsedPatch);
	Serial.print("lcdContrast : ");Serial.println(c.lcdContrast);
	Serial.print("rxChannel : ");Serial.println(c.rxChannel);
	Serial.print("txChannel : ");Serial.println(c.txChannel);
	Serial.print("version : ");Serial.println(c.version);
	Serial.print("seed : ");Serial.println(c.seed);
	
	Serial.println("=====================================");
	Serial.println("Read value : ");
	Serial.println("=====================================");
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
	len2 = strlen(c2.seed);

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
		if(c.patches[i] != c2.patches[i])
		{
			Serial.print("Reading patch ");
			Serial.print(i);
			Serial.print(" : ");
			Serial.print(c2.patches[i]);
			Serial.print(", FAILED ! EXPECTED : ");
			Serial.println(c.patches[i]);
		}
	}

	success ? Serial.println("SUCCESS !") : Serial.println("FAILED !");
}

bool testSize()
{
	unsigned short addr = E24_MAXADRESS;
	byte data[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	bool success = true;
	if(e24.write(addr, data, 2) == -1)
	{
		Serial.println("Overflow protection : SUCCESS !");
		success &= true;
	}
	else
	{
		Serial.println("Overflow protection : FAILED !");
		success &= false;
	}

	if(e24.write(addr-20, data, 10) == 10)
	{
		Serial.println("Less than one page write : SUCCESS !");
		success &= true;
	}
	else
	{
		Serial.println("Less than one page write : FAILED !");
		success &= false;
	}

	if(e24.write(addr, data, 1) == 1)
	{
		Serial.println("Last address write : SUCCESS !");
		success &= true;
	}
	else
	{
		Serial.println("Last address write : FAILED !");
		success &= false;
	}

	return success;
}

void setup()
{
	Wire.begin();
	Serial.begin(115200);
	randomSeed(123456789);
	
	bool success = true;
	//success &= testSingleByte();
	success &= testString();
	success &= testBlock();
	success &= testSize();

	if(success)
	{
		Serial.println("Summary : SUCCESS !");
	}
	else
	{
		Serial.println("Summary : FAILED !");
	}
}

void loop()
{
	
}
