#define SAMPLE_RATE 44100
#define MAX_VOICES 16
#define TUNING 440.0

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
        void on(float frequency);
        void off();

};

void Saw::set_frequency(float frequency) {
    Saw::frequency = frequency;
    Saw::phase_increment = frequency / float(SAMPLE_RATE);
}

void Saw::on(float frequency) {
    set_frequency(frequency);
    running = true;
}


void Saw::off() {
    running = false;
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
        float mtof[127];  // pre-calculated MIDI -> frequency
        Saw* notes[127];
    public:
        void init();
        void note_on(int pitch, int velocity);
        void note_off(int pitch, int velocity);
        float next_sample();
        void stop();
};

void VoiceManager::init() {
    for (auto i=0; i<MAX_VOICES; i++) {
        voices[i] = Saw();
        voices[i].running = false;
    }

    for (int x = 0; x < 127; ++x) {
        mtof[x] = pow(2.0, (x - 69.0)/12.0) * TUNING;
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

void VoiceManager::note_on(int pitch, int velocity) {
    // find inactive voice
    for (auto &v : voices) {
        if (!v.running) {
            v.on(mtof[pitch]);
            notes[pitch] = &v;
            break;
        }
    }
}


void VoiceManager::note_off(int pitch, int velocity) {
    if (notes[pitch]) {
        notes[pitch]->off();
    }
}


void VoiceManager::stop() {
    for (auto &v : voices) {
        v.running = false;
    }
}
