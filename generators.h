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
        float frequency {0.0};
    protected:
        float m_phase {0.0};
        float m_phase_increment {0.0};
};

inline void Phasor::advance_phase() {
    m_phase += phase_increment(frequency);
    m_phase = mod_phase(m_phase);
}

float Phasor::next_sample() {
    float sample = m_phase;
    advance_phase();
    return sample;
}

struct VoiceParams {
    float phase {0.0}; // start phase [0..1]
    float ratio {1.0};
};

// Voice base class
class Voice: public Phasor {
    public:
        virtual void note_on(float frequency, float velocity);
        void note_off();
        float next_sample() override;
        bool running();
        void set_ADSR(float attack, float decay, float sustain, float release);
        void set_params(VoiceParams params) {m_params = params;};
        VoiceParams m_params;
    private:
        virtual float compute_sample(float phase);
        ADSR m_env;
};

void Voice::note_on(float frequency, float velocity) {
    m_phase = m_params.phase;
    this->frequency = frequency;
    m_env.reset();
    m_env.gate(true);
}

void Voice::note_off() {
    m_env.gate(false);
}

float Voice::next_sample() {
    if (!this->running()) {
        return 0.0;
    }
    float sample = this->compute_sample(m_phase);
    sample *= m_env.process();
    this->advance_phase();
    return sample;
}

float Voice::compute_sample(float phase) {
    return zero_one_to_minus_plus(phase);
}

bool Voice::running() {
    return m_env.getState();
}

void Voice::set_ADSR(float attack, float decay, float sustain, float release) {
    m_env.setAttackRate(attack * config::SAMPLE_RATE);
    m_env.setDecayRate(decay * config::SAMPLE_RATE);
    m_env.setSustainLevel(sustain);
    m_env.setReleaseRate(release * config::SAMPLE_RATE);
    m_env.setTargetRatioA(100);
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

// naive square ------------------------------
class Square : public Voice {
    private:
        virtual float compute_sample(float phase) {
            float sample = (phase <= 0.5);
            return zero_one_to_minus_plus(sample);
        }
};

// wavetable sin
class Sine: public Voice {
    private:
        virtual float compute_sample(float phase) {
            return fast_sine(phase);
        }
};


// noise ---------------------
class Noise: public Voice {
    private:
        virtual float compute_sample(float /* phase - unused */) {
            return fast_rand_float();
        }
};


class FM : public Voice {
    public:
        void note_on(float frequency, float velocity) override;
        float k_ {0.0};
        Sine modulator_;
    private:
        float compute_sample(float phase) override;
};

void FM::note_on(float frequency, float velocity) {
    k_ = velocity / 127.0 * 10;
    modulator_.note_on(m_params.ratio * frequency, velocity);
    Voice::note_on(frequency, velocity);
}

float FM::compute_sample(float phase) {
    phase += k_ * modulator_.frequency * modulator_.next_sample();
    phase = mod_phase(phase);
    return fast_sine(phase);
}


class ResoSaw : public Voice {
    public:
        float speed_ {1.0};
        float reso_ {1.0};
    private:
        float compute_sample(float phase) override;
};

float ResoSaw::compute_sample(float phase) {
    float saw = 2.f * reso_ * (1.f - phase);
    float r = (1.0f - phase) * (1.0f - reso_);
    float res_phase = phase * speed_;
    if (res_phase < 0.5) {
        r = 1.0f - phase;
        saw = 0.0;
    }
    // advance phase to get a cosine
    res_phase += 0.25;
    res_phase = mod_phase(res_phase);

    float resonance = (-fast_sine(res_phase) + 1.0f) * r;

    return resonance + saw - 1.0f;
}


class PhaseDistortion : public Voice {
    public:
        float x_ {0.5};
    private:
        float compute_sample(float phase) override;
};

float PhaseDistortion::compute_sample(float phase) {
    if (phase <= x_) {
        phase = (1.0 - x_) / x_ * phase;
    } else {
        phase = (x_ / (1.0 - x_)) * phase + (1 - (x_ / (1.0 - x_)));
    }
    phase += 0.25;
    phase = mod_phase(phase);
    return fast_sine(phase);
}

// managing and mixing the available voices
template<typename T>
class VoiceManager : public Generator {
    private:
        T voices[config::MAX_VOICES];
        float m_mtof[127];  // pre-calculated MIDI -> frequency
        T* notes[127];
    public:
        void init();
        void note_on(int pitch, int velocity, VoiceParams params);
        void note_off(int pitch, int velocity);
        float next_sample();
        int running_voices();
        void stop();
        void set_ADSR(float attack, float decay, float sustain, float release);
};

template<typename T>
void VoiceManager<T>::init() {
    for (auto &voice : voices) {
        voice = T();
        voice.note_off();
    }

    for (int x = 0; x < 127; ++x) {
        m_mtof[x] = mtof(x);
    }
}

template<typename T>
float VoiceManager<T>::next_sample() {
    float sample {0.0};
    for (auto &voice : voices) {
        sample += voice.next_sample();
    }

    sample /= float(config::MAX_VOICES);

    return sample;
}

template<typename T>
void VoiceManager<T>::note_on(int pitch, int velocity, VoiceParams params) {
    float frequency = m_mtof[pitch];
    float volume = float(velocity) / 127.0;
    // note already playing?
    if (notes[pitch] != nullptr) {
        notes[pitch]->set_params(params);
        notes[pitch]->note_on(frequency, volume);
    // no, find inactive voice
    } else {
        for (auto &voice : voices) {
            if (!voice.running()) {
                voice.set_params(params);
                voice.note_on(frequency, volume);
                notes[pitch] = &voice;
                break;
            }
        }
    }
}

template<typename T>
void VoiceManager<T>::note_off(int pitch, int velocity) {
    if (notes[pitch]) {
        notes[pitch]->note_off();
        notes[pitch] = nullptr;
    }
}

template<typename T>
int VoiceManager<T>::running_voices() {
    int count = 0;
    for (auto &voice : voices) {
        if (voice.running()) {
            count += 1;
        }
    }
    return count;
}

template<typename T>
void VoiceManager<T>::stop() {
    for (auto &voice : voices) {
        voice.note_off();
    }
}

template<typename T>
void VoiceManager<T>::set_ADSR(float attack, float decay, float sustain, float release) {
    for (auto &voice : voices) {
        voice.set_ADSR(attack, decay, sustain, release);
    }
}
