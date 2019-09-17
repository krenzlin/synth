#include <cmath>

#include "helper.hpp"
#include "config.hpp"

// helper functions ------------------------------
float mtof(const int note) {
    return pow(2.0, (note - 69.0)/12.0) * config::TUNING;
}


std::array<float, config::WAVETABLE_SIZE> sin_table;
void create_wavetable() {
    constexpr float increment {1.0 / (config::WAVETABLE_SIZE-1)}; // incrementing SIZE-1 times
    float phase {0.0};
    for (auto i = 0; i < config::WAVETABLE_SIZE; ++i) {
        sin_table[i] = std::sin(2.0*M_PI*phase);
        phase += increment;
    }
}


float sine(float phase) {
    phase = mod_phase(phase);

    return fast_sine(phase);
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

int random_MIDI_note(const int min, const int max) {
    float rand = fast_rand_float();
    rand = minus_plus_to_zero_one(rand);
    int note = int(rand * (max - min));

    return note + min;
}
