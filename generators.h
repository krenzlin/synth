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


// Envelope Base Class
class Envelope {
    public:
        float envelope();
        void env_on();
        void env_off();
        bool running();
        void set_ADSR(float attack, float decay, float sustain, float release);
    private:
        bool m_gate {false};
        ADSR m_env;
};

float Envelope::envelope() {
    return m_env.process();
}

void Envelope::env_on() {
    m_env.reset();
    m_env.gate(true);
    m_gate = true;
}

void Envelope::env_off() {
    m_env.gate(false);
    m_gate = false;
}

bool Envelope::running() {
    return m_gate || m_env.getState();
}

void Envelope::set_ADSR(float attack, float decay, float sustain, float release) {
    m_env.setAttackRate(attack * SAMPLE_RATE);
    m_env.setDecayRate(decay * SAMPLE_RATE);
    m_env.setSustainLevel(sustain);
    m_env.setReleaseRate(release * SAMPLE_RATE);
    m_env.setTargetRatioA(100);
}


// Voice ---------
class Voice: public Phasor, public Envelope {
    public:
        void note_on(float frequency);
        void note_off();
        float next_sample() override;
    private:
        virtual float compute_sample(float phase) = 0;
};

void Voice::note_on(float frequency) {
    m_phase = 0.0;
    this->set_frequency(frequency);
    this->env_on();
}

void Voice::note_off() {
    this->env_off();
}

float Voice::next_sample() {
    if (!this->running()) {
        return 0.0;
    }
    float sample = this->compute_sample(m_phase);
    sample *= this->envelope();
    this->advance_phase();
    return sample;
}


// PolyBLEP Sawtooth ----------------------------
class Saw : public Voice {
    private:
        virtual float compute_sample(float phase) {
            float sample = phase - poly_blep(phase, m_phase_increment);
            sample = zero_one_to_minus_plus(sample);
            return sample;
        }
};


// naive sin ------------------------------
class Sin : public Voice {
    private:
        virtual float compute_sample(float phase) {
            return sin(2*M_PI*phase);
        }
};


// naive square ------------------------------
class Square : public Voice {
    private:
        virtual float compute_sample(float phase) {
            float sample = (phase <= 0.5);
            return zero_one_to_minus_plus(sample);
        }
};


// wavetable sin
class WavetableSine: public Voice {
    private:
        virtual float compute_sample(float phase) {
            return fast_sine(phase);
        }
};
