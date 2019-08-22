#pragma once

#include "config.h"
#include "generators.h"
#include "helper.h"

class AudioBase {
    virtual void process(float buffer[], int buffer_size=config::BUFFER_SIZE);
    virtual float dsp() = 0;
    virtual void calculate_parameters() {};
    virtual void init(float frequency, int velocity) {}
};


void AudioBase::process(float buffer[], int buffer_size) {
    calculate_parameters();
    for (auto i=0; i < buffer_size; i++) {
        buffer[i] = dsp();
    }
}


class SimpleSine : AudioBase {
    float phase {0.0};
    float phase_incr {0.0};

    void init(float frequency, int velocity) {
        phase_incr = phase_increment(frequency);
    }

    float dsp() {
        float sample = fast_sine(phase);
        phase += phase_incr;
        phase = mod_phase(phase);
    }
};
