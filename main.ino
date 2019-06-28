// connect a speaker to PIN 25 and GND (upper right corner)
#include "driver/i2s.h"
#include "freertos/portmacro.h"  // needed for 'portMAX_DELAY'

#define DAC1_GPIO 25
#define DAC2_GPIO 26

#define I2S_USE_BUILT_IN_DAC (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN)

#define SAMPLE_RATE 96000
#define BUFFER_SIZE 32
#define DAC_MAX_VALUE 255.0 // IMPORTANT THAT IT'S A FLOAT, OTHERWISE ALL MATH IS CASTED TO INT


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
}


void setup_I2S() {
    i2s_config_t i2s_config = {
        .mode = I2S_USE_BUILT_IN_DAC,
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, /* the DAC module will only take the 8bits from MSB */
        .channel_format = I2S_CHANNEL_FMT_ALL_RIGHT,
        .communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_I2S_MSB,
        .intr_alloc_flags = 0, // default interrupt priority
        .dma_buf_count = 2,  // number of DMA buffers (at least 2, round-robin)
        .dma_buf_len = BUFFER_SIZE, // length of each DMA buffer (in samples)
        .use_apll = false,
        .tx_desc_auto_clear = true,
    };

    esp_err_t err = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    Serial.printf("I2S install: %d\nr\r", err);

    // important otherwise no sound
    i2s_set_dac_mode(I2S_DAC_CHANNEL_RIGHT_EN);
}

Saw saw1;
Saw saw2;
Saw saw3;


void setup() {
    Serial.begin(115200);

    setup_I2S();

    saw1.set_frequency(349.2);  // F
    saw2.set_frequency(440.0);  // A
    saw3.set_frequency(523.3);  // C

}

void audio_loop(AudioObject &audio_object) {
    static unsigned int buffer[BUFFER_SIZE];
    static float sample {0.0};

    // fill buffer with samples
    for (auto i=0; i<BUFFER_SIZE; i++) {
        sample = audio_object.next_sample();
        buffer[i] = (unsigned int)(sample * DAC_MAX_VALUE);
        buffer[i] = buffer[i] << 8;
    }

    // write to I2S DMA
    static size_t bytes_written = 0;
    esp_err_t err = i2s_write(I2S_NUM_0, &buffer, 4*BUFFER_SIZE, &bytes_written, portMAX_DELAY);

    /*
    "Normally, the i2c_write_bytes function blocks. 
    You can just start up a thread that writes bytes 
    as fast as it can using that function, and the 
    blocking behaviour will make sure the timing is OK."
    from: https://www.esp32.com/viewtopic.php?t=3168#p14931
    */
}

void loop() {
    audio_loop(saw1);
}
