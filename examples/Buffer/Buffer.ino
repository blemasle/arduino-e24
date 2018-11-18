#include <Arduino.h>
#include <E24.h>

#define BUFFER_LENGTH 10
#define DATA_ADDR 0x10

E24 e24 = E24(E24Size_t::E24_512K, E24_DEFAULT_ADDR);

void setup() {
    uint8_t data[BUFFER_LENGTH];
    for(uint8_t i = 0; i < BUFFER_LENGTH; i++) {
        data[i] = i;
    }
    
    Wire.begin();
    //write the entire array
    e24.write(DATA_ADDR, data, BUFFER_LENGTH);

    //read back the array
    e24.read(DATA_ADDR, data, BUFFER_LENGTH);
}

void loop() {}