// connect a speaker to PIN 25 and GND (upper right corner)
#include "driver/i2s.h"
#include "freertos/portmacro.h"  // needed for 'portMAX_DELAY'

#define DAC1_GPIO 25
#define DAC2_GPIO 26

#define I2S_USE_BUILT_IN_DAC (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN)

#define SAMPLE_RATE 44100
#define BUFFER_SIZE 64
#define DAC_MAX_VALUE 255.0 // IMPORTANT THAT IT'S A FLOAT, OTHERWISE ALL MATH IS CASTED TO INT


void setup_I2S() {
    i2s_config_t i2s_config = {
        .mode = I2S_USE_BUILT_IN_DAC,
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, /* the DAC module will only take the 8bits from MSB */
        .channel_format = I2S_CHANNEL_FMT_ALL_RIGHT,
        .communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_I2S_MSB,
        .intr_alloc_flags = 0, // default interrupt priority
        .dma_buf_count = 2,  // ?? number of linked DMA buffers
        // according to https://www.esp32.com/viewtopic.php?t=10140#p41782
        // DMA uses multiple buffers to be filled (one to be read) and round-robins between them.
        .dma_buf_len = BUFFER_SIZE, // length of each DMA buffer (in samples)
        //.tx_desc_auto_clear = true,
        .use_apll = false
    };

    esp_err_t err = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    Serial.printf("I2S install: %d\nr\r", err);

    // important otherwise no sound
    i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);
    i2s_set_sample_rates(I2S_NUM_0, SAMPLE_RATE);
    i2s_zero_dma_buffer(I2S_NUM_0);
}

void setup() {
    Serial.begin(115200);

    setup_I2S();
}

void loop() {
    static float frequency {440.0};
    static float phase {0.0};
    static float phase_increment {float(frequency) / float(SAMPLE_RATE)};

    static unsigned int buffer[BUFFER_SIZE];

    for (auto i=0; i<BUFFER_SIZE; i++) {
        buffer[i] = (unsigned int)(phase * DAC_MAX_VALUE);
        buffer[i] = buffer[i] << 8;
        //Serial.printf("sample: %u\n\r", buffer[i]);

        phase += phase_increment;
        if (phase > 1.0) {
            phase -= 1.0;
        }
    }

    static size_t bytes_written = 0;
    esp_err_t err = i2s_write(I2S_NUM_0, &buffer, 4*BUFFER_SIZE, &bytes_written, portMAX_DELAY);
    //Serial.printf("err: %d, bytes written: %d\n\r", err, bytes_written);

    /*
    "Normally, the i2c_write_bytes function blocks. 
    You can just start up a thread that writes bytes 
    as fast as it can using that function, and the 
    blocking behaviour will make sure the timing is OK."
    from: https://www.esp32.com/viewtopic.php?t=3168#p14931
    */
}
