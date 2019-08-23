#include "doctest.h"

#include "osc.hpp"
#include "helper.hpp"


TEST_CASE("[Generator] basic") {
    // cannot use abstract base class directly
    struct Base : Generator {
        float sample() {return 0.0;};
    };

    Base gen;

    CHECK(gen.sample() == 0.0);
}


TEST_CASE( "[Noise] sample") {
    Noise noise;

    config::random_seed = 1.0;
    CHECK(noise.sample() == doctest::Approx(0.00001f));
}


TEST_CASE( "[Saw] test implementation") {
    const int steps = 100;
    float frequency = config::SAMPLE_RATE / float(steps);

    Saw osc;
    osc.bandlimit = false;
    osc.note_on(frequency, 1.0);

    for (auto i=0; i < steps; i++) {
        float value = float(i) / float(steps);
        value = zero_one_to_minus_plus(value);

        CAPTURE(i);
        CAPTURE(osc.frequency);
        CAPTURE(osc.p_incr);
        CHECK(osc.sample() == doctest::Approx(value));
    }

}


TEST_CASE( "[Voice] test running status") {
    Voice<Noise> voice;
    CHECK(voice.is_active() == false);
    voice.note_on(440.0, 127.0);
    CHECK(voice.is_active() == true);
}
