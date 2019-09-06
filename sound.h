/* 
Strangely if I name this file audio.h Arduino or the VSCode plugin won't find it.
Naming it to something that comes alphabetically after the other two imports (config.h and defs.h) solves that.
Why? I do not know and won't put any more time into it. If you know why and how to fix it, send me an email. 
Thanks,
Konrad
 */
#pragma once

#include "driver/i2s.h"
#include "freertos/FreeRTOSConfig.h" // define for xTaskCreatePinnedToCore
#include "freertos/portmacro.h"  // needed for 'portMAX_DELAY'
#include "freertos/task.h"  // TaskHandle_t, xTaskCreatePinnedToCore

#include "config.h"
#include "defs.h"
#include "osc.hpp"


TaskHandle_t audio_task;


void audio_setup() {
    i2s_config_t i2s_config = {
        .mode = I2S_USE_BUILT_IN_DAC,
        .sample_rate = config::SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, /* the DAC module will only take the 8bits from MSB */
        .channel_format = I2S_CHANNEL_FMT_ALL_RIGHT,
        .communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_I2S_MSB,
        .intr_alloc_flags = 0, // default interrupt priority
        .dma_buf_count = 2,  // number of DMA buffers (at least 2, round-robin)
        .dma_buf_len = config::BUFFER_SIZE, // length of each DMA buffer (in samples)
        .use_apll = false,
        .tx_desc_auto_clear = true,
    };

    esp_err_t err = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);

    // important otherwise no sound
    i2s_set_dac_mode(I2S_DAC_CHANNEL_RIGHT_EN);
}

void audio_loop(void* generator) {
    static unsigned int buffer[config::BUFFER_SIZE];
    static float sample {0.0};
    static size_t bytes_written = 0;

    while(1) {
        // fill buffer with samples
        for (auto i=0; i<config::BUFFER_SIZE; i++) {
            sample = ((Generator*)generator)->sample();
            sample = (sample + 1.f) * DAC_MAX_VALUE/2.f;  // trafo: [-1...+1] => [0...255]
            buffer[i] = (unsigned int)(sample);
            buffer[i] = buffer[i] << 8;  // DAC uses only first 8 of the 16bit (MSB)
        }

        // write to I2S DMA
        esp_err_t err = i2s_write(I2S_NUM_0, &buffer, sizeof(sample)*config::BUFFER_SIZE, &bytes_written, portMAX_DELAY);
    }
}

void audio_start(Generator &generator) {
    int core {0};
    if (CONFIG_ARDUINO_RUNNING_CORE == 0) {
        core = 1;
    } else {
        core = 0;
    }

    xTaskCreatePinnedToCore(
        audio_loop,     // task code
        "audio_loop",   // task name
        1000,           // usStackDepth
        &generator,      // parameters
        configMAX_PRIORITIES - 1, // Priority
        &audio_task,    // task handle */
        core);          // core id
}

void audio(Generator &generator) {
    audio_setup();
    audio_start(generator);
}
