#ifndef ARDUINO

#include <iostream>
#include <stdio.h>
#include "generators.h"


int main() {
    Saw osc;
    osc.set_frequency(440.0);

    float buffer[SAMPLE_RATE];
    
    for (auto i=0; i<SAMPLE_RATE; i++) {
        buffer[i] = osc.next_sample();
        std::cout << buffer[i] << std::endl;
    }

    std::FILE* file = std::fopen("generated.raw", "wb");
    std::fwrite(buffer, sizeof(float), SAMPLE_RATE, file);
    std::fclose(file);

    return 0;
}

#endif
