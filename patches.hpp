#include "osc.hpp"
#include "voice.hpp"

inline constexpr float dist(float x, float a=0.f) noexcept {
    return (1.f + a)*x / (1.f + a*x*x);
}

struct OrganOsc : Oscillator {
    Double<Sine, DriftingSine> h0 {};
    Double<Sine, DriftingSine> h1 {};
    Double<Sine, DriftingSine> h2 {};
    float dist_amount = 0.28f;

    void on(float frequency, float velocity) override {
        h0.on(frequency, velocity);
        h1.on(frequency*2.f, velocity*0.707f);
        h2.on(frequency*4.f, velocity*0.707f*0.707f);

        h0.mix = 0.707f;
        h1.mix = 0.707f;
        h2.mix = 0.707f;
    }

    float sample() override {
        return dist(h0.sample() + \
                h1.sample() + \
                h2.sample(), dist_amount);
    }
};

using Organ = VoiceManager< Voice<OrganOsc> >;

struct StringsOsc : Oscillator {
    Double<Saw, DriftingSaw> osc {};
    Lowpass lp;

    void on(float frequency, float velocity) override {
        lp.set_cutoff(frequency);
        osc.mix = 0.8f;
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

using Strings = VoiceManager<StringVoice>;
