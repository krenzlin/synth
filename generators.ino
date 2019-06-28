#define SAMPLE_RATE 96000
#define MAX_VOICES 16


class AudioObject {
    public:
        virtual float next_sample();
};

class Saw : public AudioObject {
    private:
        float frequency;
        float phase;
        float phase_increment;

    public:
        void set_frequency(float frequency);
        float next_sample();
};

void Saw::set_frequency(float frequency) {
    Saw::frequency = frequency;
    Saw::phase_increment = frequency / float(SAMPLE_RATE);
}

float Saw::next_sample() {
    float sample = phase;

    phase += phase_increment;
    if (phase > 1.0) {
        phase -= 1.0;
    }

    return sample;
}

class VoiceManager : public AudioObject {
    private:
        Saw voices[MAX_VOICES];
    public:
        void init();
        void play(float frequency);
        float next_sample();
};

void VoiceManager::init() {
    for (auto i=0; i<MAX_VOICES; i++) {
        voices[i] = Saw();
        voices[i].set_frequency(i*100.0 + 100);
    }
}

float VoiceManager::next_sample() {
    float sample {0.0};
    for (auto i=0; i<MAX_VOICES; i++) {
        sample += voices[i].next_sample() / MAX_VOICES;
    }

    return sample;
}