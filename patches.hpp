#include "osc.hpp"


struct Organ : Generator {
    Double<Sine, DriftingSine> h0 {};
    Double<Sine, DriftingSine> h1 {};
    Double<Sine, DriftingSine> h2 {};

    void note_on(float frequency, float velocity) {

        h0.note_on(frequency, velocity);
        h1.note_on(frequency*1.5, velocity*0.707);
        h2.note_on(frequency*2.0, velocity*0.707*0.707);


        h0.mix = 0.707;
        h1.mix = 0.707;
        h2.mix = 0.707;
    }

    float sample() {
        return h0.sample() + \
                h1.sample() + \
                h2.sample();
    }
};