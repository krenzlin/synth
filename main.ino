// connect a speaker to PIN 25 and GND (upper right corner)
#include "driver/i2s.h"
#include "freertos/portmacro.h"  // needed for 'portMAX_DELAY'

#define DAC1_GPIO 25
#define DAC2_GPIO 26

#define I2S_PORT (i2s_port_t)0
#define I2S_USE_BUILT_IN_DAC (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN)

#define SAMPLE_RATE 441000

#define BUFFER_LENGTH 255
static int buffer[BUFFER_LENGTH];



static const i2s_config_t i2s_config = {
    .mode = I2S_USE_BUILT_IN_DAC,
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, /* the DAC module will only take the 8bits from MSB */
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_I2S_MSB,
    .intr_alloc_flags = 0, // default interrupt priority
    .dma_buf_count = 8,  // ?? number of linked DMA buffers
    .dma_buf_len = 64, // length of each DMA buffer (in samples)
    //.tx_desc_auto_clear = true,
    .use_apll = false
};


void setup() {
    Serial.begin(115200);
    Serial.print("in setup\n\r");

    esp_err_t err = i2s_driver_install((i2s_port_t)I2S_PORT, &i2s_config, 0, NULL);
    Serial.print("installed:");
    Serial.print(err);
    Serial.print("\n\r");

    // important otherwise no sound
    i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);

}

void loop() {
    size_t bytes_written = 0;
    for(unsigned int i=0;i<BUFFER_LENGTH;i++)
        buffer[i] = i;
    i2s_write(I2S_PORT, &buffer, BUFFER_LENGTH, &bytes_written, portMAX_DELAY);
}
