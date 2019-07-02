#pragma once

#include <math.h>
#include "config.h"

// helper functions ------------------------------
float poly_blep(float p, float dp) {
    if (p < dp) {
        p /= dp;
        return p - p*p/2 - 0.5;
    } else if (p > 1.0 - dp) {
        p = (p - 1.0) / dp;
        return p*p/2 + p + 0.5;
    }
    return 0.0;
}

float mtof(int note) {
    return pow(2.0, (note - 69.0)/12.0) * TUNING;
}


float smoothstep(float x) {
    return x * x * (3.0 - 2.0 * x);
}

// trafo: [0..1] => [-1..+1]
inline float zero_one_to_minus_plus(float x) {
    return 2.0*x - 1.0;
}

// trafo: [0..1] => [-1..+1]
inline float minus_plus_to_zero_one(float x) {
    return (x + 1.0) / 2.0;
}

// base class Audio Generator ------------------
class Generator {
    public:
        virtual float next_sample() = 0;
};

// Phasor ----------------------------------------
// cyclic ramp from 0 to 1 at frequency
class Phasor : public Generator {
    public:
        float next_sample() override;
        void advance_phase();
        void set_frequency(float frequency);
        
    protected:
        float frequency {0.0};
        float phase {0.0};
        float phase_increment {0.0};
};

inline void Phasor::set_frequency(float frequency) {
    Phasor::frequency = frequency;
    Phasor::phase_increment = frequency / SAMPLE_RATE;
}

inline void Phasor::advance_phase() {
    phase += phase_increment;
    if (phase > 1.0) {
        phase -= 1.0;
    }
}

float Phasor::next_sample() {
    float sample = phase;
    advance_phase();
    return sample;
}


// PolyBLEP Sawtooth ----------------------------
class Saw : public Phasor {
    public:
        float next_sample() override;
        void on(float frequency);
        void off();
        bool running {false};
};

float Saw::next_sample() {
    if (!running) {
        return 0.0;
    }
    float sample = phase - poly_blep(phase, phase_increment);
    sample = (sample - 0.5) * 2.0;  // transform from [0; 1] to  [-1; 1]
    advance_phase();
    return sample;
};

void Saw::on(float frequency) {
    set_frequency(frequency);
    running = true;
}

void Saw::off() {
    running = false;
}


// naive sin ------------------------------
class Sin : public Phasor {
    public:
        float next_sample() override;
};

float Sin::next_sample() {
    float sample = sin(2*M_PI*phase);
    advance_phase();
    return sample;
}


// naive square ------------------------------
class Square : public Phasor {
    public:
        float next_sample() override;
};

float Square::next_sample() {
    float sample = (phase <= 0.5);  // 0 .. 0.5 => 1 & .5 .. 1 => 0
    advance_phase();
    return sample;
}


// smoothstep sin ------------------------------
// approx. sin via smoothstep
// 1. generate triangle wave
// 2. move so that
class SmoothSin : public Phasor {
    public:
        SmoothSin() {phase = 0.25;}  // move phase so sin starts on zero
        float next_sample() override;
};

float SmoothSin::next_sample() {
    float sample = 2*phase;
    if (phase > 0.5) {
        sample = 2.0 - sample;
    }
    sample = smoothstep(sample);
    sample = zero_one_to_minus_plus(sample);
    advance_phase();
    return sample;
}
