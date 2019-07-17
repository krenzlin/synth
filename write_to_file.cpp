#ifndef ARDUINO

#include <iostream>
#include <stdio.h>
#include "config.h"
#include "generators.h"

float simulate_8bit(float value) {
    return value;
    return int(value * 128) / 128.0;
}

void write_to_file(Generator &gen, float ms, std::FILE* file) {
    int n_samples = int(config::SAMPLE_RATE * ms/1000.0);
    float buffer[n_samples];

    for (auto i=0; i<n_samples; i++) {
        buffer[i] = simulate_8bit(gen.next_sample());
    }
    std::fwrite(buffer, sizeof(float), n_samples, file);
}

float SR = config::SAMPLE_RATE;

int main() {
    create_wavetable();
    VoiceManager<WavetableSine> osc;
    osc.init();
    osc.set_ADSR(0.0, 0.5, 0.0, 0.0);

    std::FILE* file = std::fopen("generated.raw", "wb");
    if (true) {
        osc.note_on(69, 127);
        osc.note_on(30, 127);
        write_to_file(osc, 250, file);
        osc.note_off(30, 127);

        osc.note_on(30, 127);
        write_to_file(osc, 250, file);
        osc.note_off(69, 127);
        osc.note_off(30, 127);


        osc.note_on(72, 127);
        osc.note_on(30, 127);
        write_to_file(osc, 250, file);
        osc.note_off(30, 127);

        osc.note_on(30, 127);
        write_to_file(osc, 250, file);
        osc.note_off(72, 127);
        osc.note_off(30, 127);

        osc.note_on(71, 127);
        osc.note_on(30, 127);
        write_to_file(osc, 250, file);
        osc.note_off(71, 127);
        osc.note_off(30, 127);

        osc.note_on(72, 127);
        osc.note_on(30, 127);
        write_to_file(osc, 250, file);
        osc.note_off(72, 127);
        osc.note_off(30, 127);

        osc.note_on(74, 127);
        osc.note_on(30, 127);
        write_to_file(osc, 250, file);
        osc.note_off(74, 127);
        osc.note_off(30, 127);

    }
    if (false) {
        for (auto n=20; n<127; n++) {
            osc.note_on(n, 127);
            write_to_file(osc, 50, file);
            osc.note_off(n, 127);
        }
    }

    std::fclose(file);
    std::cout << "sample rate "  << SR << std::endl;

    return 0;
}

#endif
