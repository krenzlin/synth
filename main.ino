// connect a speaker to PIN 25 and GND (upper right corner)
#include "driver/i2s.h"
#include "freertos/portmacro.h"  // needed for 'portMAX_DELAY'
#include <MIDI.h>

#include "config.h"
#include "defs.h"


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
    Serial.printf("I2S install: %d\n\r", err);

    // important otherwise no sound
    i2s_set_dac_mode(I2S_DAC_CHANNEL_RIGHT_EN);
}

VoiceManager vm;

TaskHandle_t audio_task;

MIDI_CREATE_DEFAULT_INSTANCE();

void handleNoteOn(byte channel, byte pitch, byte velocity) {
    digitalWrite(LED, HIGH);
    vm.note_on(int(pitch), int(velocity));
    delay(10);
    digitalWrite(LED, LOW);
}

void handleNoteOff(byte channel, byte pitch, byte velocity) {
    vm.note_off(int(pitch), int(velocity));
}

void setup() {
    Serial.begin(115200);
    Serial.print("setup() running on core ");
    Serial.println(xPortGetCoreID());

    setup_I2S();

    pinMode(LED, OUTPUT);

    MIDI.setHandleNoteOn(handleNoteOn);
    MIDI.setHandleNoteOff(handleNoteOff);
	MIDI.begin(MIDI_CHANNEL_OMNI);

    vm.init();

    xTaskCreatePinnedToCore(
        audio_loop,                  /* pvTaskCode */
        "audio_loop",            /* pcName */
        1000,                   /* usStackDepth */
        NULL,                   /* pvParameters */
        1,                      /* uxPriority */
        &audio_task,                 /* pxCreatedTask */
        0);                     /* xCoreID */


}

void audio_loop(void * parameter) {
    static unsigned int buffer[BUFFER_SIZE];
    static float sample {0.0};
    static size_t bytes_written = 0;

    while(1) {
        // fill buffer with samples
        for (auto i=0; i<BUFFER_SIZE; i++) {
            sample = vm.next_sample();
            buffer[i] = (unsigned int)(sample * DAC_MAX_VALUE);
            buffer[i] = buffer[i] << 8;
        }

        // write to I2S DMA
        esp_err_t err = i2s_write(I2S_NUM_0, &buffer, 4*BUFFER_SIZE, &bytes_written, portMAX_DELAY);
    }
    /*
    "Normally, the i2c_write_bytes function blocks. 
    You can just start up a thread that writes bytes 
    as fast as it can using that function, and the 
    blocking behaviour will make sure the timing is OK."
    from: https://www.esp32.com/viewtopic.php?t=3168#p14931
    */
}

void loop() {
    MIDI.read();
}
