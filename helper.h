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
    return pow(2.0, (note - 69.0)/12.0) * config::TUNING;
}


float smoothstep(float x) {
    return x * x * (3.0 - 2.0 * x);
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

float sin_table[config::WAVETABLE_SIZE];
void create_wavetable() {
    constexpr float increment {M_PI*2.0 / (config::WAVETABLE_SIZE-1)}; // incrementing SIZE-1 times
    float phase {0.0};
    for (auto i = 0; i < config::WAVETABLE_SIZE; ++i) {
        sin_table[i] = sin(phase);
        phase += increment;
    }
}

// expects phase to be between 0.0 and 1.0
float fast_sine(float phase) {
    int index = int(phase * float(config::WAVETABLE_SIZE));
    return sin_table[index];
}


/* Fast random implementation [-1..+1]
see https://en.wikipedia.org/wiki/Lehmer_random_number_generator

X_k+1 = a * X_k mod m

m = 2^31 − 1 = 2,147,483,647 (a Mersenne prime M31)
a = 7^5 = 16,807

"Five years later, we see no need to alter our response other than to suggest the use of the multiplier a = 48271 in place of 16807."
"For now, we feel comfortable continuing to use a = 16807."
a = 48271

Type int of random_seed and a is important as the cast to integer acts as the modulo.
First divide by m and then floor result.

*/
static int random_seed {1};
float fast_rand_float() {
    constexpr int a = 16807;
    constexpr float m_1 = 4.6566129e-010f; // 1/(2^31-1) = 1/2147483647

    random_seed *= a;
    return (float)random_seed * m_1;
}


int random_MIDI_note(int min=0, int max=127) {
    float rand = fast_rand_float();
    rand = minus_plus_to_zero_one(rand);
    int note = int(rand * (max - min));

    return note + min;
}
