#pragma once

#include "ADSR.h"

#include "config.h"
#include "helper.h"

// base class Audio Generator ------------------
class Generator {
    public:
        virtual float next_sample() = 0;
};

// Phasor ----------------------------------------
// cyclic ramp from 0 to 1 at frequency
class Phasor : public Generator {
    public:
        float next_sample() override;
        void advance_phase();
        void set_frequency(float frequency) {m_frequency = frequency;}
        
    protected:
        float m_frequency {0.0};
        float m_phase {0.0};
        float m_phase_increment {0.0};
};

inline void Phasor::advance_phase() {
    m_phase_increment = m_frequency / SAMPLE_RATE;
    m_phase += m_phase_increment;
    if (m_phase > 1.0) {
        m_phase -= 1.0;
    }
}

float Phasor::next_sample() {
    float sample = m_phase;
    advance_phase();
    return sample;
}


// PolyBLEP Sawtooth ----------------------------
class Saw : public Phasor {
    public:
        float next_sample() override;
        void on(float frequency);
        void off();
        bool running();
        void set_ADSR(float attack, float decay, float sustain, float release);
    private:
        bool m_gate {false};
        ADSR m_env;
};

bool Saw::running() {
    return m_gate && m_env.getState();
}


void Saw::set_ADSR(float attack, float decay, float sustain, float release) {
    m_env.setAttackRate(attack);
    m_env.setDecayRate(decay);
    m_env.setSustainLevel(sustain);
    m_env.setReleaseRate(release);
    m_env.setTargetRatioA(100);
}

float Saw::next_sample() {
    if (!running()) {
        return 0.0;
    }
    float sample = m_phase - poly_blep(m_phase, m_phase_increment);
    sample *= m_env.process();
    sample = zero_one_to_minus_plus(sample);
    advance_phase();
    return sample;
};

void Saw::on(float frequency) {
    set_frequency(frequency);
    m_env.reset();
    m_env.gate(true);
    m_gate = true;
}

void Saw::off() {
    m_env.gate(false);
    m_gate = false;
}


// naive sin ------------------------------
class Sin : public Phasor {
    public:
        float next_sample() override;
};

float Sin::next_sample() {
    float sample = sin(2*M_PI*m_phase);
    advance_phase();
    return sample;
}


// naive square ------------------------------
class Square : public Phasor {
    public:
        float next_sample() override;
};

float Square::next_sample() {
    float sample = (m_phase <= 0.5);  // 0 .. 0.5 => 1 & .5 .. 1 => 0
    advance_phase();
    return sample;
}


// smoothstep sin ------------------------------
// approx. sin via smoothstep
// 1. generate triangle wave
// 2. move so that
class SmoothSin : public Phasor {
    public:
        SmoothSin() {m_phase = 0.25;}  // move phase so sin starts on zero
        float next_sample() override;
};

float SmoothSin::next_sample() {
    float sample = 2*m_phase;
    if (m_phase > 0.5) {
        sample = 2.0 - sample;
    }
    sample = smoothstep(sample);
    sample = zero_one_to_minus_plus(sample);
    advance_phase();
    return sample;
}
