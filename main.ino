// connect a speaker to PIN 25 and GND (upper right corner)
#include "driver/i2s.h"
#include "freertos/portmacro.h"  // needed for 'portMAX_DELAY'

#define DAC1_GPIO 25
#define DAC2_GPIO 26

#define I2S_USE_BUILT_IN_DAC (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN)

#define SAMPLE_RATE 44100
#define BUFFER_SIZE 64
#define DAC_MAX_VALUE 255.0 // IMPORTANT THAT IT'S A FLOAT, OTHERWISE ALL MATH IS CASTED TO INT


static const i2s_config_t i2s_config = {
    .mode = I2S_USE_BUILT_IN_DAC,
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, /* the DAC module will only take the 8bits from MSB */
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_I2S_MSB,
    .intr_alloc_flags = 0, // default interrupt priority
    .dma_buf_count = 2,  // ?? number of linked DMA buffers
    // according to https://www.esp32.com/viewtopic.php?t=10140#p41782
    // DMA uses multiple buffers to be filled (one to be read) and round-robins between them.
    .dma_buf_len = BUFFER_SIZE, // length of each DMA buffer (in samples)
    //.tx_desc_auto_clear = true,
    .use_apll = false
};


class Sawtooth {
    public:
        Sawtooth(float sample_rate): sr{sample_rate} {};
        void SetFrequency(float frequency);
        void compute(unsigned int buffer[], size_t len);
        float next_sample();
    private:
        float sr;
        float frequency;
        float phase {0.0};
        float phase_increment;
};


static Sawtooth saw{SAMPLE_RATE};


void setup() {
    Serial.begin(115200);

    esp_err_t err = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    Serial.printf("installed: %d\nr\r", err);

    // important otherwise no sound
    i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);

    i2s_set_sample_rates(I2S_NUM_0, SAMPLE_RATE);

    i2s_zero_dma_buffer(I2S_NUM_0);

    saw.SetFrequency(440.0);
}

void loop() {
    static unsigned int buffer[BUFFER_SIZE];

    saw.compute(buffer, BUFFER_SIZE);
    Serial.printf("buffer[10]: %u\n\r", buffer[10]);

    static size_t bytes_written = 0;
    esp_err_t err = i2s_write(I2S_NUM_0, buffer, BUFFER_SIZE, &bytes_written, portMAX_DELAY);
    Serial.printf("err: %d, bytes written: %d\n\r", err, bytes_written);

    /*
    "Normally, the i2c_write_bytes function blocks. 
    You can just start up a thread that writes bytes 
    as fast as it can using that function, and the 
    blocking behaviour will make sure the timing is OK."
    from: https://www.esp32.com/viewtopic.php?t=3168#p14931
    */
}


void Sawtooth::SetFrequency(float frequency) {
    Sawtooth::frequency = frequency;
    Sawtooth::phase_increment = Sawtooth::frequency / Sawtooth::sr;
}

float Sawtooth::next_sample() {
    phase += phase_increment;
    while (phase > 1.0) {
        phase -= 1.0;
    }

    return phase;
}

void Sawtooth::compute(unsigned int buffer[], size_t len) {
    for (auto i=0; i<len; i++) {
        float sample = Sawtooth::next_sample()*DAC_MAX_VALUE;
        buffer[i] = (unsigned int)sample;
    }
}
