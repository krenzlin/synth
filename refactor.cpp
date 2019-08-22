#ifndef ARDUINO

#include <iostream>
#include <stdio.h>

#include "config.h"
#include "osc.hpp"

#define print(msg) std::cout << msg << std::endl;

int main() {
    create_wavetable();
    VoiceManager<Saw> osc;
    
    osc.note_on(69, 127);
    osc.note_on(80, 127);

    for (auto i=0; i < 10; i++) {
        print(osc.sample());
    }

}

#endif
