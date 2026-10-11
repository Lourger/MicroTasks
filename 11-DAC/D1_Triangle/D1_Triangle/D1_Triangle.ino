#include <Arduino.h>

// ---------- Pins ----------
#define DAC_PIN        25           // DAC1 output
#define AMP_POT_PIN    34           // pot 1: amplitude
#define FREQ_POT_PIN   35           // pot 2: frequency

// ---------- Ranges and timing ----------
#define MIN_AMPLITUDE  1            // DAC codes; 0 would mean dividing by zero below
#define MAX_AMPLITUDE  255
#define MIN_FREQ_HZ    1
#define MAX_FREQ_HZ    500
#define POT_READ_US    100000       // read the pots every 100 ms
#define POT_SAMPLES    5            // average of 5 readings

int amplitude = 128;                // peak DAC code: the wave goes 0 .. amplitude
int frequencyHz = 250;             // until the pots are first read
unsigned long stepUs = 15;          // time per DAC step (1,000,000 / (2 x 128 x 250))
int currentValue = 0;
bool rising = true;
unsigned long lastStepUs = 0;
unsigned long lastPotReadUs = 0;

int readAverage(int pin) {
  long sum = 0;
  for (int i = 0; i < POT_SAMPLES; i++) {
    sum += analogRead(pin);
  }
  return sum / POT_SAMPLES;
}

void readPots() {
  amplitude   = map(readAverage(AMP_POT_PIN), 0, 4095, MIN_AMPLITUDE, MAX_AMPLITUDE);
  frequencyHz = map(readAverage(FREQ_POT_PIN), 0, 4095, MIN_FREQ_HZ, MAX_FREQ_HZ);
  // one period = 2 x amplitude steps (up, then down)
  stepUs = 1000000UL / (2UL * amplitude * frequencyHz);
  if (currentValue > amplitude) {   // stay inside the new peak
    currentValue = amplitude;
  }
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  readPots();
}

void loop() {
  unsigned long now = micros();

  if (now - lastPotReadUs >= POT_READ_US) {
    lastPotReadUs = now;
    readPots();
  }

  if (now - lastStepUs >= stepUs) {  // time for the next step
    lastStepUs = now;
    dacWrite(DAC_PIN, currentValue);
    if (rising) {
      currentValue++;
      if (currentValue >= amplitude) {
        rising = false;
      }
    } else {
      currentValue--;
      if (currentValue <= 0) {
        rising = true;
      }
    }
  }
}
