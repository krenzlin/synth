#ifndef ARDUINO

#include <iostream>
#include <stdio.h>
#include "config.hpp"
#include "osc.hpp"
#include "patches.hpp"

float SR = config::SAMPLE_RATE;

int main() {
    create_wavetable();
 
    Organ osc {};
    osc.note_on(60, 127);
    for (auto i=0; i<SR*5; i++) {
        osc.sample();
    }
}

#endif
