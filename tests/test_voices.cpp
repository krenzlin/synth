#include "doctest.h"

#include "voice.hpp"


TEST_CASE("[Voice] test running status") {
    Voice<Noise> voice;
    CHECK(voice.is_active() == false);
    voice.note_on(69, 127);
    CHECK(voice.is_active() == true);
}

TEST_CASE("[Voice] test if voice gets inactive when envelope runs out") {
    Voice<Noise> voice;
    voice.env.setAttackRate(0.f);
    voice.env.setDecayRate(1.f);  // <-- !! decay in 1 sample, BUT see below
    voice.env.setSustainLevel(0.f);
    voice.env.setReleaseRate(0.f);



    voice.note_on(69, 127);
    CAPTURE(voice.env.output);
    CAPTURE(voice.env.state);


    CHECK(voice.is_active() == true);

    // has to go through attack and decay --> 2 samples

    voice.sample(); // attack
    voice.sample(); // decay
    //voice.sample(); // sustain


    CHECK(voice.is_active() == false);
}


TEST_CASE("[Voice] test interface") {
    Voice<Noise> voice;
    voice.is_active();
    voice.note_on(69, 127);
    voice.note_off(127);
    voice.sample();
    voice.handle_control_change(11, 127);
}


TEST_CASE("[VoiceManager] test interface") {
    VoiceManager< Voice<Noise> > vm;
    vm.note_on(69, 127);
    vm.note_off(69, 127);
    vm.sample();
    vm.handle_control_change(11, 127);
}
