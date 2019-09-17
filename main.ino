// connect a speaker to PIN 25 and GND (upper right corner)
#include <Arduino.h>
#include <MIDI.h>

#include "config.hpp"
#include "defs.h"
#include "sound.h"
#include "patches.hpp"

// declare Synth/Audio object in global namespace to keep it alive for the audio thread
Organ vm {};

MIDI_CREATE_DEFAULT_INSTANCE();

void handleNoteOn(byte channel, byte pitch, byte velocity) {
    digitalWrite(LED, HIGH);
    vm.note_on(int(pitch), int(velocity));
    delay(10);
    digitalWrite(LED, LOW);
}

void handleNoteOff(byte channel, byte pitch, byte velocity) {
    vm.note_off(int(pitch), int(velocity));
}

void handleControlChange(byte channel, byte number, byte value) {
    float data = float(value)/127.0;
}

void blink(int N) {
    for (auto i=0; i<N; i++) {
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
    }
}

void setup() {
    pinMode(LED, OUTPUT);

    blink(2);
    create_wavetable();
    blink(1);

    MIDI.setHandleNoteOn(handleNoteOn);
    MIDI.setHandleNoteOff(handleNoteOff);
    MIDI.setHandleControlChange(handleControlChange);
    MIDI.begin(MIDI_CHANNEL_OMNI);

    audio(vm);
}

void loop() {
    MIDI.read();
    for (auto i=0; i<5; i++) {
        vm.note_on(random_MIDI_note(40, 80), 127);
        delay(500);
    }
    for (auto i=0; i<127; i++) {
        vm.note_off(i, 127);
    }
    delay(1000);
}
