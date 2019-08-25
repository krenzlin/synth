#pragma once

#include "ADSR.h"
#include "config.hpp"
#include "helper.hpp"

struct Generator {
    virtual float sample() = 0;
    virtual void note_on(float /* frequency */, float /* velocity */) {};
};


struct Saw : Generator {
    float frequency {0.0};
    float p_incr {0.0};
    float phase {0.0};
    bool bandlimit {true};

    void note_on(float frequency, float /* velocity */) override {
        frequency = frequency;
        p_incr = phase_increment(frequency);
    }

    float sample() {
        float sample = phase;
        if (bandlimit) {
            sample -= poly_blep(phase, p_incr);
        }
        sample = zero_one_to_minus_plus(sample);

        phase += p_incr;
        if (phase > 1.0) {
            phase -= 1.0;
        }
        return sample;
    }
};

struct Sine : Generator {
    float frequency {0.0};
    float velocity {1.0};
    float phase {random_phase()};

    void note_on(float frequency, float velocity) override {
        this->frequency = frequency;
        this->velocity = velocity;
    }

    float sample() {
        float sample = fast_sine(phase) * velocity;;

        phase += phase_increment(frequency);
        phase = mod_phase(phase);
        return sample;
    }
};

struct DriftingSine : Sine {
    float drift {0.05};
    bool sign {false};
    int steps {0};
    int max_steps {5};

    float sample() {
        float sample = fast_sine(phase);

        phase += phase_increment(frequency + drift);
        if (phase > 1.0) {
            phase -= 1.0;

            // hit a new cycle -> update frequency drift
            frequency += drift;
            steps--;

            if (steps <= 0) {
                steps = int(minus_plus_to_zero_one(fast_rand_float()) * max_steps);
                drift *= -1.0;
            }
        }
        return sample;
    }
};

struct Noise : Generator {
    float sample() {
        return fast_rand_float();
    }
};

template<typename T>
struct Double : Generator {
    T osc1 {};
    T osc2 {};
    float detune {0.0};
    float mix {0.9};

    void note_on(float frequency, float velocity) {
        osc1.note_on(frequency - detune, velocity);
        osc2.note_on(frequency + detune, velocity);
    }

    float sample() {
        return mix * osc1.sample() + (1.0 - mix) * osc2.sample();
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

    float sample() {
        if (!is_active()) {
            return 0.0;
        }

        return float(osc.sample() * env.process());
    }

    void note_on(int pitch, int velocity) {
        float frequency = mtof(pitch);
        osc.note_on(frequency, float(velocity) / 127.0);

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
