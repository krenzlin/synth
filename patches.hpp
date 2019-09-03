#include "osc.hpp"
#include "voice.hpp"

inline float dist(float x, float a=0.0) {
    return (1.0 + a)*x / (1.0 + a*x*x);
}

struct OrganOsc : Oscillator {
    Double<Sine, DriftingSine> h0 {};
    Double<Sine, DriftingSine> h1 {};
    Double<Sine, DriftingSine> h2 {};
    float dist_amount = 0.28;

    void on(float frequency, float velocity) override {
        h0.on(frequency, velocity);
        h1.on(frequency*2.0, velocity*0.707);
        h2.on(frequency*4.0, velocity*0.707*0.707);

        h0.mix = 0.707;
        h1.mix = 0.707;
        h2.mix = 0.707;
    }

    float sample() override {
        return dist(h0.sample() + \
                h1.sample() + \
                h2.sample(), dist_amount);
    }
};

typedef VoiceManager< Voice<OrganOsc> > Organ;

struct StringsOsc : Oscillator {
    Double<Saw, DriftingSaw> osc {};
    Lowpass lp;

    void on(float frequency, float velocity) override {
        lp.set_cutoff(frequency);
        osc.mix = 0.8;
        osc.osc1.phase = random_phase();
        osc.osc2.phase = random_phase();
        osc.osc2.max_steps = 20;
        osc.on(frequency, velocity);
    }

    float sample() override {
        return lp.process(osc.sample());
    }
};


struct StringVoice : Voice<StringsOsc> {
    StringVoice() {
        env.setAttackRate(0.1 * config::SAMPLE_RATE);
        env.setReleaseRate(0.1 * config::SAMPLE_RATE);
    }
};

typedef VoiceManager<StringVoice> Strings;
