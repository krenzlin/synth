#include "osc.hpp"

inline float dist(float x, float a=0.0) {
    return (1.0 + a)*x / (1.0 + a*x*x);
}

struct Organ : Generator {
    Double<Sine, DriftingSine> h0 {};
    Double<Sine, DriftingSine> h1 {};
    Double<Sine, DriftingSine> h2 {};
    float dist_amount = 0.28;

    void note_on(float frequency, float velocity) {

        h0.note_on(frequency, velocity);
        h1.note_on(frequency*2.0, velocity*0.707);
        h2.note_on(frequency*4.0, velocity*0.707*0.707);

        h0.mix = 0.707;
        h1.mix = 0.707;
        h2.mix = 0.707;
    }

    float sample() {
        return dist(h0.sample() + \
                h1.sample() + \
                h2.sample(), dist_amount);
    }
};


struct Strings : Generator {
    Double<Saw, DriftingSaw> osc {};
    Lowpass lp;

    void note_on(float frequency, float velocity) {
        lp.set_cutoff(frequency);
        osc.mix = 0.8;
        osc.osc1.phase = random_phase();
        osc.osc2.phase = random_phase();
        osc.osc2.max_steps = 20;
        osc.note_on(frequency, velocity);
    }

    float sample() {
        return lp.process(osc.sample());
    }
};
