#include "doctest.h"

#include "voice.hpp"


TEST_CASE("[Voice] test running status") {
    Voice<Noise> voice;
    CHECK(voice.is_active() == false);
    voice.note_on(69, 127);
    CHECK(voice.is_active() == true);
}


TEST_CASE("[Voice] test interface") {
    Voice<Noise> voice;
    voice.is_active();
    voice.note_on(69, 127);
    voice.note_off(127);
    voice.sample();
    voice.handle_control_change(11, 127);
}