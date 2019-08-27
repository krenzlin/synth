#pragma once

namespace config {
    static const float SAMPLE_RATE {44100.0};  // FLOAT !!!
    static const int BUFFER_SIZE {32};

    static const int MAX_VOICES {10};
    static const float TUNING {440.0};

    static const int WAVETABLE_SIZE {2048};

    static int random_seed {1};
}
