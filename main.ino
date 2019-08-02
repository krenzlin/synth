// connect a speaker to PIN 25 and GND (upper right corner)
#include <Arduino.h>
#include <MIDI.h>

#include "config.h"
#include "defs.h"
#include "sound.h"
#include "generators.h"

// declare Synth/Audio object in global namespace to keep it alive for the audio thread
VoiceManager<FM> vm {};
VoiceParams tone {};
VoiceParams kick {};

//MIDI_CREATE_DEFAULT_INSTANCE();

void handleNoteOn(byte channel, byte pitch, byte velocity) {
    digitalWrite(LED, HIGH);
    vm.note_on(int(pitch), int(velocity), tone);
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

    blink(2);
    create_wavetable();
    blink(1);

    //MIDI.setHandleNoteOn(handleNoteOn);
    //MIDI.setHandleNoteOff(handleNoteOff);
    //MIDI.begin(MIDI_CHANNEL_OMNI);

    vm.init();
    vm.set_ADSR(0.5, 0.0, 1.0, 1.0);

    audio(vm);
}

void loop() {
    //MIDI.read();
    int note1 = random_MIDI_note(60, 72);
    int note2 = random_MIDI_note(60, 72);
    int note3 = random_MIDI_note(60, 72);


    vm.note_on(note1, 127, tone);
    delay(250);
    vm.note_on(note2, 127, tone);
    delay(250);
    vm.note_on(note3, 127, tone);
    delay(250);
    vm.note_off(note1, 127);
    delay(50);
    vm.note_off(note2, 127);
    delay(50);
    vm.note_off(note3, 127);
    delay(50);
}
