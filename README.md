[![Build Status](https://travis-ci.org/blemasle/arduino-e24.svg?branch=master)](https://travis-ci.org/blemasle/arduino-e24)
[![License](https://img.shields.io/badge/license-MIT%20License-blue.svg)](http://doge.mit-license.org)

# E24

This library allows to store and read arbitrary data from the Microship's [24LCxxx](https://www.microchip.com/wwwproducts/en/en010828) series using I2C. It supports all the available EEPROM sizes from 8K to 512K, although only 256K and 512K have been tested.

Each instance only use two bytes of memory. 

## Features
 * Single byte read & write
 * Multiple bytes read & write
 * Structured block read & write
 * Adaptative buffers size to match chip pages boundaries

## Usage
Unlike most Arduino library, no default instance is created when the library is included. It's up to you to create one with the appropriate chip I2C address.

```cpp
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
```
