#pragma once

#include "ADSR.h"
#include "config.h"
#include "helper.h"

struct Generator {
    virtual double sample() = 0;
};


struct Saw : Generator {
    double frequency {0.0};
    double phase {0.0};

    double sample() {
        double p_incr = phase_increment(frequency);
        double sample =  phase - poly_blep(phase, p_incr);
        sample = zero_one_to_minus_plus(sample);

        phase += p_incr;
        phase = mod_phase(phase);

        return sample;
    }
};

struct Sine : Generator {
    double frequency {0.0};
    double phase {0.0};

    double sample() {
        double sample = fast_sine(phase);

        phase += phase_increment(frequency);
        phase = mod_phase(phase);
        return sample;
    }
};

struct Noise : Generator {
    double sample() {
        return fast_rand_float();
    }
};


template<typename T>
struct Voice : Generator {
    T osc {};
    ADSR env {};
    bool active {false};

    bool is_active() {
        return env.getState();
    }

    double sample() {
        if (!is_active()) {
            return 0.0;
        }

        return double(osc.sample() * env.process());
    }

    void note_on(int pitch, int velocity) {
        double frequency = mtof(pitch);
        osc.frequency = frequency;

        env.reset();
        env.gate(true);
    }

    void note_off(int velocity) {
        env.gate(false);
    }
};


template<typename T>
struct VoiceManager : Generator {
    Voice<T> voices[config::MAX_VOICES];
    Voice<T>* notes[127];

    VoiceManager() {
        for (auto &voice : voices) {
            voice = Voice<T> {};
        }
    }

    double sample() {
        double sample = 0.0;

        for (auto &voice : voices) {
            if (voice.is_active()) {
                sample += voice.sample();
            }
        }
        sample /= double(config::MAX_VOICES);

        return sample;
    }

    void note_on(int pitch, int velocity) {
        for (auto &voice : voices) {
            if (!voice.is_active()) {
                voice.note_on(pitch, velocity);
                notes[pitch] = &voice;
                break;
            }
        }
    }

    void note_off(int pitch, int velocity) {
        if (notes[pitch]) {
            notes[pitch]->note_off(velocity);
            notes[pitch] = nullptr;
        }
    }
};
