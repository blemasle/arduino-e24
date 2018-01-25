#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif


struct Config {
	char seed[5]; //config check, must always be : CONFIG_SEED. If not, config will be rewriten from scratch
	char version[5]; //firmware version 
	byte rxChannel;
	byte txChannel;
	byte lastUsedPatch;
	byte lcdContrast;
	byte patches[128];
};