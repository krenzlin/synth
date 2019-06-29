#define SAMPLE_RATE 96000
#define MAX_VOICES 16

// base audio class
class AudioObject {
    public:
        virtual float next_sample();
        bool running {false};
};


float poly_blep(float p, float dp) {
    if (p < dp) {
        p /= dp;
        return p - p*p/2 - 0.5;
    } else if (p > 1.0 - dp) {
        p = (p - 1.0) / dp;
        return p*p/2 + p + 0.5;
    }
    return 0.0;
}

// sawtooth generator
// till now a naive modulo implementation
class Saw : public AudioObject {
    private:
        float frequency {0.0};
        float phase {0.0};
        float phase_increment {0.0};

    public:
        void set_frequency(float frequency);
        float next_sample();
};

void Saw::set_frequency(float frequency) {
    Saw::frequency = frequency;
    Saw::phase_increment = frequency / float(SAMPLE_RATE);
}

float Saw::next_sample() {
    if (!running) {
        return 0.0;
    }

    float sample = phase - poly_blep(phase, phase_increment);

    phase += phase_increment;
    if (phase > 1.0) {
        phase -= 1.0;
    }

    return sample;
}


// managing and mixing the available voices
class VoiceManager : public AudioObject {
    private:
        Saw voices[MAX_VOICES];
    public:
        void init();
        void note_on(float frequency);
        float next_sample();
        void stop();
};

void VoiceManager::init() {
    for (auto i=0; i<MAX_VOICES; i++) {
        voices[i] = Saw();
        voices[i].running = false;
    }
}

float VoiceManager::next_sample() {
    float sample {0.0};
    for (auto i=0; i<MAX_VOICES; i++) {
        sample += voices[i].next_sample();
    }

    sample /= MAX_VOICES;

    return sample;
}

void VoiceManager::note_on(float frequency) {
    // find inactive voice
    for (auto &v : voices) {
        if (!v.running) {
            v.set_frequency(frequency);
            v.running = true;
            break;
        }
    }
}

void VoiceManager::stop() {
    for (auto &v : voices) {
        v.running = false;
    }
}
