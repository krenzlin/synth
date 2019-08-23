#pragma once

namespace config {
    inline const float SAMPLE_RATE {44100.0};  // FLOAT !!!
    inline const int BUFFER_SIZE {32};

    inline const int MAX_VOICES {10};
    inline const float TUNING {440.0};

    inline const int WAVETABLE_SIZE {2048};

    inline int random_seed {1};
}
