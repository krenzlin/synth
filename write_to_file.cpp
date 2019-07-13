#ifndef ARDUINO

#include <iostream>
#include <stdio.h>
#include "config.h"
#include "generators.h"

float simulate_8bit(float value) {
    //return value;
    return int(value * 128) / 128.0;
}

float SR = config::SAMPLE_RATE;

int main() {
    create_wavetable();
    VoiceManager<WavetableSine> osc;
    osc.init();
    osc.note_on(69, 127);
    osc.set_ADSR(0.0, 0.5, 0.0, 0.0);



    float sample {0.0};
    int len {int(2*SR)};
    float buffer[len];
    
    std::FILE* file = std::fopen("generated.raw", "wb");

    for (auto i=0; i<SR; i++) {
        sample = osc.next_sample();
        buffer[i] = simulate_8bit(sample);
    }
    std::fwrite(buffer, sizeof(float), SR, file);

    osc.note_off(69, 127);
    osc.note_on(72, 127);
    osc.note_on(77, 127);
    for (auto i=0; i<SR; i++) {
        sample = osc.next_sample();
        buffer[i] = simulate_8bit(sample);
    }


    std::fwrite(buffer, sizeof(float), SR, file);
    std::fclose(file);
    std::cout << "sample rate "  << SR << std::endl;

    return 0;
}

#endif
