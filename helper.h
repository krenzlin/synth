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
    //return (x - 0.5) * 2.0;
    return 2.0*x - 1.0;
}

float sin_table[WAVETABLE_SIZE];
void create_wavetable() {
    constexpr float increment {M_PI*2.0 / (WAVETABLE_SIZE-1)}; // incrementing SIZE-1 times
    float phase {0.0};
    for (auto i = 0; i < WAVETABLE_SIZE; ++i) {
        sin_table[i] = sin(phase);
        phase += increment;
    }
}
