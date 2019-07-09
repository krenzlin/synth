#ifndef ARDUINO

#include <iostream>
#include <stdio.h>
#include "config.h"
#include "generators.h"

float simulate_8bit(float value) {
    return int(value * 128) / 128.0;
}


int main() {
    Saw osc;
    osc.on(440.0);
    osc.set_ADSR(0.5*SAMPLE_RATE, 0.2*SAMPLE_RATE, 0.3, 0.5*SAMPLE_RATE);

    float sample {0.0};
    int len {int(2*SAMPLE_RATE)};
    float buffer[len];
    
    std::FILE* file = std::fopen("generated.raw", "wb");

    for (auto i=0; i<SAMPLE_RATE; i++) {
        sample = osc.next_sample();
        buffer[i] = simulate_8bit(sample);
    }
    std::fwrite(buffer, sizeof(float), SAMPLE_RATE, file);

    osc.off();
    for (auto i=0; i<SAMPLE_RATE; i++) {
        buffer[i] = osc.next_sample();
    }


    std::fwrite(buffer, sizeof(float), SAMPLE_RATE, file);
    std::fclose(file);
    std::cout << "sample rate "  << SAMPLE_RATE << std::endl;

    return 0;
}

#endif
