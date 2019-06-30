
#define SAMPLE_RATE 44100
#define BUFFER_SIZE 32

#define MAX_VOICES 16
#define TUNING 440.0

#define DAC_MAX_VALUE 255.0 // IMPORTANT THAT IT'S A FLOAT, OTHERWISE ALL MATH IS CASTED TO INT

// pins
#define LED 2

#define DAC1_GPIO 25
#define DAC2_GPIO 26

#define I2S_USE_BUILT_IN_DAC (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN)
