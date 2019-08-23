#include "doctest.h"

#include <math.h>

#include "helper.hpp"

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


TEST_CASE( "[sine] test phase wrapping function") {
    create_wavetable();

    CHECK(sine(0.25) == sine(1.25));
    CHECK(sine(0.25) == sine(-0.75));
}


TEST_CASE( "[helpers::random_MIDI_note] w/o arguments") {
    for (auto i=0; i<100; i++) {
        int note = random_MIDI_note();
        CAPTURE(note);
        CHECK(note >= 0);
        CHECK(note <= 127);
    }
}

TEST_CASE( "[helpers::random_MIDI_note] setting min and max note") {
    for (auto i=0; i<100; i++) {
        int note = random_MIDI_note(60, 72);
        CAPTURE(note);
        CHECK(note >= 60);
        CHECK(note <= 72);
    }
}
