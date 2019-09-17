#pragma once

#include <array>

#include "config.hpp"

#ifndef ARDUINO
#include <iostream>
#define PRINT(msg) std::cout << (msg) << std::endl
#define SHOW(x) PRINT(#x " = " << (x))
#else
#define PRINT(msg)
#define SHOW(x)
#endif

// helper functions ------------------------------
inline float phase_increment(const float frequency, const float sr=config::SAMPLE_RATE) {
    return frequency / sr;
}

inline float mod_phase(float phase, const float mod=1.0f) {
    while (phase > mod) {
        phase -= mod;
    }
    while (phase < 0.0f) {
        phase += mod;
    }
    return phase;
}

inline float poly_blep(float p, float dp) {
    if (p < dp) {
        p /= dp;
        return p - p*p/2.f - 0.5f;
    } else if (p > 1.0f - dp) {
        p = (p - 1.0f) / dp;
        return p*p/2.f + p + 0.5f;
    }
    return 0.0f;
}

// trafo: [0..1] => [-1..+1]
inline float zero_one_to_minus_plus(const float x) {
    //return (x - 0.5) * 2.0;
    return 2.f*x - 1.f;
}

// trafo: [-1..+1] => [0..1]
inline float minus_plus_to_zero_one(const float x) {
    return (x + 1.f) / 2.f;
}

extern std::array<float, config::WAVETABLE_SIZE> sin_table;
void create_wavetable();

// expects phase to be between 0.0 and 1.0
inline float fast_sine(const float phase) {
    int index = int(phase * float(config::WAVETABLE_SIZE - 1));
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

int random_MIDI_note(const int min=0, const int max=127);

float mtof(const int note);
