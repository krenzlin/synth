#pragma once

#include "config.hpp"
#include "helper.hpp"

struct Generator {
    virtual float sample() = 0;
};

struct Oscillator : Generator {
    virtual void on(float /* frequency */, float /* velocity */) {};
};


struct Lowpass {
    float alpha {0.0};
    float y {0.0};

    void set_cutoff(float f_c) {
        float w_c = f_c / config::SAMPLE_RATE;
        this->alpha = w_c / (1.0 + w_c);
    }

    float process(float x) {
        y += this->alpha * (x - y);
        return y;
    }
};

struct Saw : Oscillator {
    float frequency {0.0};
    float p_incr {0.0};
    float phase {0.0};
    bool bandlimit {true};

    void on(float frequency, float /* velocity */) override {
        this->frequency = frequency;
        this->p_incr = phase_increment(this->frequency);
    }

    float sample() override {
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

struct DriftingSaw : Saw {
    float drift {0.0};
    int steps {0};
    int max_steps {5};

    float sample() override {
        float sample = phase;
        if (bandlimit) {
            sample -= poly_blep(phase, p_incr);
        }
        sample = zero_one_to_minus_plus(sample);

        phase += p_incr;
        if (phase > 1.0) {
            phase -= 1.0;

            steps--;

            if (steps <= 0) {
                steps = int(minus_plus_to_zero_one(fast_rand_float()) * max_steps);
                drift = fast_rand_float();
                if ((drift > 1.0) | (drift < -1.0)) {
                    drift = 0.0;
                }
                p_incr = phase_increment(frequency + drift);
            }

        }
        return sample;
    }
};

struct Sine : Oscillator {
    float frequency {0.0};
    float velocity {1.0};
    float phase {random_phase()};
    float p_incr {0.0};

    void on(float frequency, float velocity) override {
        this->frequency = frequency;
        this->velocity = velocity;
        this->p_incr = phase_increment(this->frequency);
    }

    float sample() override {
        float sample = fast_sine(phase) * velocity;;

        phase += p_incr;
        if (phase > 1.0) {
            phase -= 1.0;
        }
        return sample;
    }
};

struct DriftingSine : Sine {
    float drift {0.0};
    bool sign {false};
    int steps {0};
    int max_steps {5};

    float sample() override {
        float sample = fast_sine(phase);

        phase += p_incr;
        if (phase > 1.0) {
            phase -= 1.0;

            steps--;
            if (steps <= 0) {
                steps = int(minus_plus_to_zero_one(fast_rand_float()) * max_steps);
                drift = fast_rand_float();
                if ((drift > 1.0) | (drift < -1.0)) {
                    drift = 0.0;
                }
            }

            p_incr = phase_increment(frequency + drift);
        }

        return sample;
    }
};

struct Noise : Oscillator {
    float sample() override {
        return fast_rand_float();
    }
};

template<typename T1, typename T2>
struct Double : Oscillator {
    T1 osc1 {};
    T2 osc2 {};
    float detune {0.0};
    float mix {0.5};

    void on(float frequency, float velocity) override {
        osc1.on(frequency - detune, velocity);
        osc2.on(frequency + detune, velocity);
    }

    float sample() override {
        return mix * osc1.sample() + (1.0 - mix) * osc2.sample();
    }
};
