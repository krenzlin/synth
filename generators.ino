#include "generators.h"
#include "config.h"


// managing and mixing the available voices
class VoiceManager : public Generator {
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
        voices[i].note_off();
        voices[i].set_ADSR(0.5*SAMPLE_RATE, 0.2*SAMPLE_RATE, 0.2, 0.2);
    }

    for (int x = 0; x < 127; ++x) {
        mtof[x] = pow(2.0, (x - 69.0)/12.0) * TUNING;
    }
}

float VoiceManager::next_sample() {
    float sample {0.0};
    for (auto &v : voices) {
        sample += v.next_sample();
    }

    sample /= MAX_VOICES;

    return sample;
}

void VoiceManager::note_on(int pitch, int velocity) {
    // find inactive voice
    for (auto &v : voices) {
        if (!v.running()) {
            v.note_on(mtof[pitch]);
            notes[pitch] = &v;
            break;
        }
    }
}


void VoiceManager::note_off(int pitch, int velocity) {
    if (notes[pitch]) {
        notes[pitch]->note_off();
    }
}


void VoiceManager::stop() {
    for (auto &v : voices) {
        v.note_off();
    }
}
