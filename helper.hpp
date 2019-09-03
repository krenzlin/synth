#pragma once

#include <cmath>

#include "config.hpp"

#ifndef ARDUINO
#include <iostream>
#define PRINT(msg) std::cout << msg << std::endl
#define SHOW(x) PRINT(#x " = " << x)
#else
#define PRINT(msg)
#define SHOW(x)
#endif

// helper functions ------------------------------
inline float phase_increment(float frequency, const float sr=config::SAMPLE_RATE) {
    return frequency / sr;
}

inline float mod_phase(float phase, const float mod=1.0) {
    while (phase > mod) {
        phase -= mod;
    }
    while (phase < 0.0) {
        phase += mod;
    }
    return phase;
}

inline float poly_blep(float p, float dp) {
    if (p < dp) {
        p /= dp;
        return p - p*p/2 - 0.5;
    } else if (p > 1.0 - dp) {
        p = (p - 1.0) / dp;
        return p*p/2 + p + 0.5;
    }
    return 0.0;
}

// trafo: [0..1] => [-1..+1]
inline float zero_one_to_minus_plus(float x) {
    //return (x - 0.5) * 2.0;
    return 2.0*x - 1.0;
}

// trafo: [-1..+1] => [0..1]
inline float minus_plus_to_zero_one(float x) {
    return (x + 1.0) / 2.0;
}

extern float sin_table[config::WAVETABLE_SIZE];
void create_wavetable();

// expects phase to be between 0.0 and 1.0
inline float fast_sine(float phase) {
    int index = int(phase * float(config::WAVETABLE_SIZE));
    return sin_table[index];
}

// expects phase to be between 0.0 and 1.0
float sine(float phase);

/* Fast random implementation [-1..+1]
see https://en.wikipedia.org/wiki/Lehmer_random_number_generator

X_k+1 = a * X_k mod m


m = 2^31 − 1 = 2,147,483,647 (a Mersenne prime M31)
a = 7^5 = 16,807
*/
// Return: [-1..+1]
inline float fast_rand_float() {
    constexpr int a = 16807;
    constexpr float m_1 = 4.6566129e-010f; // 1/(2^31-1) = 1/2147483647

    config::random_seed *= a;
    return float(config::random_seed) * m_1;
}

inline float random_phase() {
    return minus_plus_to_zero_one(fast_rand_float());
}

int random_MIDI_note(int min=0, int max=127);

float mtof(int note);
