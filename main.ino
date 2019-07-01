// connect a speaker to PIN 25 and GND (upper right corner)
#include <Arduino.h>
#include <MIDI.h>

#include "config.h"
#include "defs.h"
#include "sound.h"

// declare Synth/Audio object in global namespace to keep it alive for the audio thread
VoiceManager vm;

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

    blink(3);

    MIDI.setHandleNoteOn(handleNoteOn);
    MIDI.setHandleNoteOff(handleNoteOff);
    MIDI.begin(MIDI_CHANNEL_OMNI);

    vm.init();
    vm.note_on(69, 127);
    audio(vm);
}

void loop() {
    MIDI.read();
}
