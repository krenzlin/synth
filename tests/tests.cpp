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


TEST_CASE( "[sine] test phase wrapping function") {
    create_wavetable();

    CHECK(sine(0.25) == sine(1.25));
    CHECK(sine(0.25) == sine(-0.75));
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
    float frequency = config::SAMPLE_RATE / float(steps);
    float velocity = 127.f;
    Saw osc;
    osc.note_on(frequency, velocity);

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
    voice.note_on(440.0, 127.f);
    CHECK(voice.running() == true);
}

TEST_CASE( "[generators::Voice] set parameters") {
    Voice voice;
    VoiceParams vp;

    vp.phase = 0.0;
    voice.set_params(vp);
    voice.note_on(440.0, 127.f);
    CHECK(voice.next_sample() == -1.0);

    // start half-way through cycle
    vp.phase = 0.5;
    voice.set_params(vp);
    voice.note_on(440.0, 127.f);
    CHECK(voice.next_sample() == 0.0);

}

TEST_CASE( "[generators::VoiceManager] anew note on should reset running voice") {
    VoiceManager<Voice> vm {};
    VoiceParams params {};

    CHECK(vm.running_voices() == 0);

    vm.note_on(60, 127, params);

    CHECK(vm.running_voices() == 1);

    vm.note_on(61, 127, params);

    CHECK(vm.running_voices() == 2);

    vm.note_on(60, 127, params);

    // should reset running voice on pitch 60
    CHECK(vm.running_voices() == 2);
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
