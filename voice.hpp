#include "ADSR.h"
#include "osc.hpp"


template<typename T>
struct Voice : Generator {
    T osc {};
    ADSR env {};

    bool is_active() {
        return env.getState();
    }

    float sample() {
        if (!is_active()) {
            return 0.0;
        }

        return float(osc.sample() * env.process());
    }

    void note_on(int note, int velocity) {
        float frequency = mtof(note);
        osc.on(frequency, float(velocity) / 127.0);

        env.reset();
        env.gate(true);
    }

    void note_off(int velocity) {
        env.gate(false);
    }

    void handle_control_change(int number, int value) {
    }
};


template<typename T>
struct VoiceManager : Generator {
    T voices[config::MAX_VOICES];
    T* notes[127];

    VoiceManager() {
        for (auto &voice : voices) {
            voice = T {};
        }
    }

    float sample() {
        float sample = 0.0;

        for (auto &voice : voices) {
            if (voice.is_active()) {
                sample += voice.sample();
            }
        }
        sample /= float(config::MAX_VOICES);

        return sample;
    }

    void note_on(int note, int velocity) {
        for (auto &voice : voices) {
            if (!voice.is_active()) {
                voice.note_on(note, velocity);
                notes[note] = &voice;
                break;
            }
        }
    }

    void note_off(int note, int velocity) {
        if (notes[note]) {
            notes[note]->note_off(velocity);
            notes[note] = nullptr;
        }
    }

    void handle_control_change(int number, int value) {
        for (auto &voice : voices) {
            voice.handle_control_change(number, value);
        }
    }
};
