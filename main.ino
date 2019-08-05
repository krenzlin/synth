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

MIDI_CREATE_DEFAULT_INSTANCE();

void handleNoteOn(byte channel, byte pitch, byte velocity) {
    digitalWrite(LED, HIGH);
    vm.note_on(int(pitch), int(velocity), tone);
    delay(10);
    digitalWrite(LED, LOW);
}

void handleNoteOff(byte channel, byte pitch, byte velocity) {
    vm.note_off(int(pitch), int(velocity));
}

void handleControlChange(byte channel, byte number, byte value) {
    float data = float(value)/127.0;
    tone.ratio = (float) int(data * 3.0);
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

    vm.init();
    //vm.set_ADSR(0.001, 0.5, 0.0, 0.0);

    audio(vm);
}

void loop() {
    MIDI.read();
}
