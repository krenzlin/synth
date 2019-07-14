#ifndef ARDUINO

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <math.h>

#include "helper.h"
#include "generators.h"


TEST_CASE( "[zero_one_to_minus_plus] transform [0..1] to [-1..+1]") {
    CHECK( zero_one_to_minus_plus(0) == -1 );
    CHECK( zero_one_to_minus_plus(1) == +1 );
    CHECK( zero_one_to_minus_plus(0.5) == 0.0 );
}


TEST_CASE( "[fast_sine] wavetable sine approx. within 1%") {
    const int steps = 1000;

    create_wavetable();

    for (auto i=0; i <= steps; i++) {
        float value = float(i) / float(steps);
        float correct = sin(value * 2.0 * M_PI);
        float approx = fast_sine(value);

        CAPTURE(value);
        CHECK(doctest::Approx(approx).epsilon(0.01) == correct);
    }
}

TEST_CASE( "[generators::Phasor] test implementation") {
    const int steps = 100;
    Phasor osc;
    osc.frequency = config::SAMPLE_RATE / float(steps);

    for (auto i=0; i < steps; i++) {
        float value = float(i) / float(steps);

        CAPTURE(i);
        CHECK(osc.next_sample() == doctest::Approx(value));
    }
}


TEST_CASE( "[generators::Saw] test implementation") {
    const int steps = 100;
    Saw osc;
    osc.note_on(config::SAMPLE_RATE / float(steps));

    for (auto i=0; i < steps; i++) {
        float value = float(i) / float(steps);
        value = zero_one_to_minus_plus(value);

        CAPTURE(i);
        CHECK(osc.next_sample() == doctest::Approx(value));
    }
}

TEST_CASE( "[generators::Voice] test running status") {
    Voice voice;
    CHECK(voice.running() == false);
    voice.note_on(440.0);
    CHECK(voice.running() == true);
}

#endif // ARDUINO
