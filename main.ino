// connect a speaker to PIN 25 and GND (upper right corner)

#define DAC1_GPIO 25

void setup() {
}

void loop() {
    for(int i=0;i<256;i++)
        dacWrite(DAC1_GPIO,i);
}
