#include <Arduino.h>

struct Config {
	char seed[5]; //config check, must always be : CONFIG_SEED. If not, config will be rewriten from scratch
	char version[5]; //firmware version 
	byte rxChannel;
	byte txChannel;
	byte lastUsedPatch;
	byte lcdContrast;
	byte patches[128];
};