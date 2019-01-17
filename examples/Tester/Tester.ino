/**
 * Provides a serial interface to test functions of the 24 library
 */

#include <E24.h>
#include <Wire.h>
#include <ArduinoLog.h>

#define SOMETHING_SIZE	10						///< size of the something string in block_t
#define DATA_SIZE		10						///< size of the data array in block_t
#define POWER_I2C		A0						///< pin used to deliver power to the 24LC chip on my test circuit
#define E24_SIZE    	E24Size_t::E24_512K		///< size of the 24LC chip
#define E24_P_SIZE		E24_PAGE_SIZE(E24_SIZE)	///< size of a page of the 24LC
#define E24_MAX_ADDR 	E24_MAX_ADDRESS(E24_SIZE)

#define SERIAL_BAUDRATE 38400		   			///< controls the serial baudrate between the arduino and the computer
#define BUFFER_SIZE 128 						///< size of the reading buffer on the main program side
#define NL "\n"

#if defined(__AVR__)
	#define STRING_IS(s1, s2) strcasecmp_P(buffer, PSTR(s2)) == 0
	#define BUFFER_IS(s) STRING_IS(buffer, s)
	#define BUFFER_IS_P(s) strcasecmp_P(buffer, s) == 0

	#define PRINT(s) Serial.print(F(s))
	#define PRINT_LN(s) Serial.println(F(s))
	#define S_PROGMEM PROGMEM
    #define S_F(x)   F(x)
    #define TO_F(x) (reinterpret_cast<const __FlashStringHelper*>(x))
#else
	#define BUFFER_IS(s) strcasecmp(buffer, s) == 0
	#define BUFFER_IS(s) STRING_IS(buffer, s)
	#define BUFFER_IS_P(s) strcasecmp(buffer, s) == 0

	#define PRINT(s) Serial.print(s)
	#define PRINT_LN(s) Serial.println(s)
	#define S_PROGMEM
    #define S_F(x)  x
    #define TO_F(x) x
#endif

const char UNRECOGNIZED[] S_PROGMEM = "Unrecognized : %s" NL;
const char UNKNOWN[] S_PROGMEM = "Unknown value";

const char SINGLE[] S_PROGMEM = "SINGLE";
const char RANGE[] S_PROGMEM = "RANGE";
const char PAGE[] S_PROGMEM = "PAGE";
const char BLOCK[] S_PROGMEM = "BLOCK";

typedef struct block_t {
	char something[SOMETHING_SIZE];
	uint8_t version;
	uint8_t data[DATA_SIZE];
};

E24 e24 = E24(E24_SIZE, E24_DEFAULT_ADDR);
char buffer[BUFFER_SIZE];

void usage() {
	PRINT_LN("============= E24 Tester =============");
	PRINT_LN("?\tHelp");
	
	PRINT_LN("");

	PRINT_LN("info\t\t\t\tDisplay info about the current chip");
	PRINT_LN("reset\t\t\t\tReset the device");

	PRINT_LN("");

	PRINT_LN("fill range [addr] [length] [value] \tFill n bytes with value starting at addr");
	PRINT_LN("fill page [page] [value] \t\tFill an entire page with a value");

	PRINT_LN("");

	PRINT_LN("read single [addr]\t\t\tRead a single byte at the specified address");
	PRINT_LN("read range [addr] [length]\t\tRead n bytes from addr");
	PRINT_LN("read page [page]\t\t\tRead the specified page");
	PRINT_LN("read block [addr]\t\t\tRead a test block from the specified address");
	
	PRINT_LN("");

	PRINT_LN("write single [addr] [value]\t\tWrite value at the specified address");
	PRINT_LN("write range [addr]\t\t\tWrite a char array to the specified address");
	PRINT_LN("write block [addr]\t\t\tWrite a test block value at the specified address");

	PRINT_LN("");
	PRINT_LN("");
}

/**
 * Read the next line or word into the buffer.
 * Return true if this was the last word of the line.
 */
bool readNext(bool line = false) {
	char c;
	char *p = buffer;
	memset(p, 0, BUFFER_SIZE);

	do {
		while(!Serial.available()) delay(100);

		c = Serial.read();
		if((!line && isspace(c)) || c == '\n') break;

		*p = c;
		p++;
	} while(true);

	*p = '\0';
	return c == '\n';
}

/**
 * Read an unsigned long from the next line or word.
 * Return true if this was the last parameter of the line.
 */
bool readNextInt(uint16_t * result, bool line = false) {
	bool last = readNext(line);
	*result = strtol(buffer, NULL, 10);

	return last;
}

/**
 * Read an unsigned int from the next line or word.
 * Return true if this was the last parameter of the line.
 */
bool readNextByte(uint8_t * result, bool line = false) {
	int16_t tmpResult;

	bool last = readNextInt(&tmpResult, line);
	*result = (uint8_t)tmpResult;

	return last;
}

// n digits leading zero
char *p2dig(uint16_t v, uint8_t mode, uint8_t n = 2) {
	static char s[5];
	uint8_t i = 0;
	uint8_t base = 0;

	switch (mode) {
	case HEX:
		base = 16;
		break;
	case DEC:
		base = 10;
		break;
	}

	while(n - (i + 1) > 0 && v < pow(base, n - (i + 1))) s[i++] = '0';
	itoa(v, &s[i], base);
	s[n + 1] = '\0';

	return s;
}

void showContent(uint16_t addr, uint16_t length) {
#define TABLE_WIDTH	16

	uint16_t read = 0;
	uint16_t j, k, regCount, lineStart = 0;
	uint8_t buf[E24_P_SIZE];
	uint16_t end = start + length;

	if(end < addr) end = E24_MAX_ADDR;

	//header
	PRINT(NL);
	PRINT("Offset ");
	for (uint8_t i = 0; i < TABLE_WIDTH; i++) {
		Serial.print(p2dig(i, HEX));
		PRINT(" ");
	}

	for (uint16_t i = start; i < end && i >= start; i += read) { // i < start => reached the end of the chip
		//new line with address
		PRINT(NL);
		lineStart = (i / TABLE_WIDTH) * TABLE_WIDTH;
		Serial.print(p2dig(lineStart, HEX, 4));
		PRINT(":  ");

		//padding
		for (j = 0; j < i - lineStart; j++) {
			PRINT("   ");
		}

		read = e24.read(i, buf, min(end, lineStart + TABLE_WIDTH) - i);
		//data
		for (j = 0; j < read; j++) {
			if (regCount < E24_MAX_ADDR) {
				PRINT("");
				Serial.print(p2dig(buf[j], HEX));
				PRINT(" ");
				k = j; // save this index for the next loop
				regCount++;
			}
			else PRINT("   ");
		}

		//padding
		for (;j < TABLE_WIDTH; j++) {
			PRINT("   ");
		}

		//data as char
		PRINT("  ");
		for (j = 0; j <= k; j++) {
			if (isalnum(buf[j]) || ispunct(buf[j])) {
				PRINT("");
				Serial.print((char)buf[j]);
			}
			else PRINT(".");
			PRINT(" ");
		}
	}

	PRINT(NL);
	PRINT(NL);

}

void unrecognized() {
	Log.error(TO_F(UNRECOGNIZED), buffer);
	while(Serial.available()) Serial.read();
}

void info() {
	Log.notice(S_F("24LC chip" NL));
	Log.notice(S_F("address\t: %X" NL), E24_DEFAULT_ADDR);
	Log.notice(S_F("size\t\t: %d" NL), e24.getSize());
	Log.notice(S_F("page size\t: %d" NL), E24_P_SIZE);
}

void reset() {
	uint16_t addr = 0;

	Log.notice(S_F("Erasing the whole chip" NL));
	do {
		addr += e24.fill(addr, 0, E24_P_SIZE);
		Serial.print('.');		
	} while (addr < E24_MAX_ADDR);
}

void fill() {
	uint16_t addr;
	uint8_t value;
	uint16_t written;
	bool last = readNext();

	if(BUFFER_IS_P(RANGE)) {
		uint16_t length;

		readNextInt(&addr);
		readNextInt(&length);
		readNextByte(&value);

		written = e24.fill(addr, value, length);
	}
	else if(BUFFER_IS_P(PAGE)) {
		
		readNextInt(&addr);
		readNextByte(&value);

		addr *= E24_P_SIZE;
		written = e24.fill(addr, value, E24_P_SIZE);
	}
	else {
		unrecognized();
		return;
	}

	Log.notice(S_F("Filled %d bytes @ %X with %d" NL), written, addr, value);
}

void read() {
	uint16_t addr;
	readNext();

	if(BUFFER_IS_P(SINGLE)) {
		uint8_t value;

		readNextInt(&addr);
		value = e24.read(addr);
		Log.notice(S_F("Read @ %X : %X" NL), addr, value);
	}
	else if(BUFFER_IS_P(RANGE)) {
		uint16_t length;

		readNextInt(&addr);
		readNextInt(&length);
		showContent(addr, length);
	}
	else if(BUFFER_IS_P(PAGE)) {
		readNextInt(&addr);

		addr *= E24_P_SIZE;
		showContent(addr, E24_P_SIZE);
	}
	else if (BUFFER_IS_P(BLOCK)) {
		block_t value;

		readNextInt(&addr);
		e24.readBlock(addr, value);
		value.something[SOMETHING_SIZE] = '\0'; //not normaly needed but this is playground so better be safe

		Log.notice(S_F("Read block @%X" NL));
		Log.notice(S_F("something : %s" NL), value.something);
		Log.notice(S_F("version %d" NL), value.version);
		Log.notice(S_F("data :" NL));
		for(uint8_t i = 0; i < DATA_SIZE; i++) {
			Serial.print(p2dig(value.data[i], HEX));
			PRINT(" ");
		}
		PRINT(NL);
	}
	else {
		unrecognized();
		return;
	}
}

void write() {
	uint16_t addr;
	uint16_t written;
	readNext();

	if(BUFFER_IS_P(SINGLE)) {
		uint8_t value;

		readNextInt(&addr);
		readNextByte(&value);
		e24.write(addr, value);
		Log.notice("Wrote %X @ %X", value, addr);
	}
	else if(BUFFER_IS_P(RANGE)) {
		readNextInt(&addr);

        PRINT("[value] ?");
		readNext(true);
		
		written = e24.write(addr, buffer, strlen(buffer));
		Log.notice(S_F("Wrote %d bytes @ %X" NL), written, addr);
	}
	else if(BUFFER_IS("BLOCK")) {
		block_t b;
		uint8_t version;

		readNextInt(&addr);

        PRINT("[something] ?");
		readNext(true);
		strlcpy(b.something, buffer, SOMETHING_SIZE);

		PRINT("[version] ?");
		readNextByte(&version, true);
		b.version = version;

		PRINT("[data] ?");
		readNext(true);
		memcpy(b.data, buffer, DATA_SIZE);

		written = e24.writeBlock(addr, b);
		Log.notice(S_F("Wrote %d bytes @ %X" NL), written, addr);
	}
	else {
		unrecognized();
		return;
	}

}

void setup() {
	while(!Serial);

	Serial.begin(SERIAL_BAUDRATE);
	Log.begin(LOG_LEVEL_NOTICE, &Serial);

	digitalWrite(POWER_I2C, HIGH);
	pinMode(POWER_I2C, OUTPUT);

	Wire.begin();

	usage();
}

void loop() {
	readNext();

	if(BUFFER_IS("?")) usage();
	else if(BUFFER_IS("info")) info();
	else if(BUFFER_IS("reset")) reset();
	else if(BUFFER_IS("fill")) fill();
	else if(BUFFER_IS("read")) read();
	else if(BUFFER_IS("write")) write();
	else {
		unrecognized();
		return;
	}

	Log.notice(F("Done" NL));

}