#include <array>

#include "ADSR.h"
#include "osc.hpp"


template<typename T>
struct Voice {
    T osc {};
    ADSR env {};

    bool is_active() {
        return env.getState();
    }

    float sample() {
        return osc.sample() * env.process();
    }

    void note_on(const int note, const int velocity) {
        float frequency = mtof(note);
        osc.on(frequency, float(velocity) / 127.f);

        env.reset();
        env.gate(true);
    }

    void note_off(int velocity=127) {
        env.gate(false);
    }

    void handle_control_change(const int /* number */, const int /* value */) {}
};


template<typename T>
struct VoiceManager {
    std::array<T, config::MAX_VOICES> voices {};
    std::array<T*, 127> notes {};

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

    void note_on(const int note, const int velocity=127) {
        for (auto &voice : voices) {
            if (!voice.is_active()) {
                voice.note_on(note, velocity);
                notes[note] = &voice;
                break;
            }
        }
    }

    void note_off(const int note, const int velocity=127) {
        if (notes[note]) {
            notes[note]->note_off(velocity);
            notes[note] = nullptr;
        }
    }

    void handle_control_change(const int number, const int value) {
        for (auto &voice : voices) {
            voice.handle_control_change(number, value);
        }
    }
};
