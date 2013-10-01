#include <E24LC256.h>
#include <Wire.h>

#define E24LC256_ADDR 0x50
E24LC256 e24lc = E24LC256(E24LC256_ADDR);

void testReadValue(unsigned short addr, char assertValue)
{
	byte value = e24lc.read(addr);
	Serial.print("Reading ");
	Serial.print(addr);
	Serial.print(" : ");
	Serial.print((char)value);
	if(value == assertValue)
	{
		Serial.println(", SUCCESS !");
	}
	else
	{
		Serial.println(", FAILED !");
	}
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

	delay(3000);
	
	Serial.println("=====================================");
	Serial.println("Testing arbitrary bytes write/read...");
	char str_data[]={"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nulla nunc quam, rutrum ut nisl vitae, porta laoreet purus. Nunc eget lorem consequat, mollis magna quis, interdum erat cras amet.\n"};	
	int len = strlen(str_data);
	//char* str_read_data = (char*)malloc(len);

	//addr = (unsigned short)random(0x7FFF);
	addr = 136;
	e24lc.write(addr, (byte*)str_data, len);
	
	for(int i = 0; i < len; i += 10)
	{
		testReadValue(addr + i, str_data[i]);
	}

	e24lc.read(addr, (byte*)str_data, len);

	Serial.println("Read string value : ");
	Serial.println(str_data);
}

void loop()
{
	
}
