#ifndef ARDUINO

#include <iostream>
#include <stdio.h>
#include "config.hpp"
#include "osc.hpp"

float simulate_8bit(float value) {
    return value;
    return int(value * 128) / 128.0;
}

void write_to_file(Generator &gen, float ms, std::FILE* file) {
    int n_samples = int(config::SAMPLE_RATE * ms/1000.0);

    for (auto i=0; i<n_samples; i++) {
        float sample = simulate_8bit(gen.sample());
        std::fwrite(&sample, sizeof(float), 1, file);
    }
}

float SR = config::SAMPLE_RATE;

int main() {
    create_wavetable();
    VoiceManager<Saw> osc {};

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

    std::fclose(file);
    std::cout << "sample rate "  << SR << std::endl;

    return 0;
}

#endif
