#include "generators.h"
#include "config.h"

// base audio class
class AudioObject {
    public:
        virtual float next_sample();
        bool running {false};
};


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
