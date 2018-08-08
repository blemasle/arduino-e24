# E24
This library allows to store and read arbitrary data from the Microship's [24LCxxx](https://www.microchip.com/wwwproducts/en/en010828) series using I2C. It supports all the available EEPROM sizes, although only 256K and 512K have been tested.

Each instance only use two bytes of memory. 
> No default instance is created when the library is included

## Features
 * Single byte read & write
 * Multiple bytes read & write using optimum buffer sizes to match the chip pages
 * Structured block read & write using template
