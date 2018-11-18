#include <Arduino.h>
#include <E24.h>

#define CONFIG_ADDR 0xF8

struct config_t {
    uint8_t seed;
    char version[5];
};

E24 e24 = E24(E24Size_t::E24_512K, E24_DEFAULT_ADDR);

void setup() {
    config_t config = {};
    config.seed = 123;
    strcpy(config.version, "1.00");

    Wire.begin();
    //write a structured block to the suplied address
    e24.writeBlock(CONFIG_ADDR, config);

    //read back the block into the variable
    e24.readBlock(CONFIG_ADDR, config);
}

void loop() {}