#include <Arduino.h>
#include <math.h>

// ---------- Pins ----------
#define DAC_PIN        25           // DAC1 output
#define AMP_POT_PIN    34           // pot 1: amplitude
#define FREQ_POT_PIN   35           // pot 2: frequency
#define BUTTON_PIN     32           // button to GND (internal pull-up): next waveform

// ---------- Ranges and timing ----------
#define TABLE_SIZE     256          // samples per period
#define MIN_FREQ_HZ    10.0
#define MAX_FREQ_HZ    1000.0
#define POT_READ_US    100000       // read the pots every 100 ms
#define POT_SAMPLES    5
#define LOCKOUT_MS     200          // ignore the button for 200 ms after a press

enum Waveform { SINE, SQUARE, TRIANGLE };
const char *const WAVE_NAMES[3] = {"SINE", "SQUARE", "TRIANGLE"};

uint8_t sineTable[TABLE_SIZE];
Waveform waveform = SINE;           // starts as a sine wave
int amplitude = 127;                // 1..255: scales every sample
unsigned long stepUs = 15;          // 250 Hz until the pots are first read
uint8_t waveIndex = 0;              // 0..255: where we are in the period (wraps by itself)
unsigned long lastStepUs = 0;
unsigned long lastPotReadUs = 0;
unsigned long lastPressMs = 0;

int readAverage(int pin) {
  long sum = 0;
  for (int i = 0; i < POT_SAMPLES; i++) {
    sum += analogRead(pin);
  }
  return sum / POT_SAMPLES;
}

void readPots() {
  amplitude = map(readAverage(AMP_POT_PIN), 0, 4095, 1, 255);
  // exponential: 10 Hz at one end, 100 Hz in the middle, 1000 Hz at the other end
  double frequencyHz = MIN_FREQ_HZ * pow(MAX_FREQ_HZ / MIN_FREQ_HZ, readAverage(FREQ_POT_PIN) / 4095.0);
  stepUs = (unsigned long)(1000000.0 / (TABLE_SIZE * frequencyHz));
}

uint8_t sampleAt(uint8_t i) {       // full-scale sample 0..255 for the current waveform
  switch (waveform) {
    case SINE:   return sineTable[i];
    case SQUARE: return (i < 128) ? 255 : 0;
    default:     return (i < 128) ? i * 2 : (255 - i) * 2;   // TRIANGLE: up 128 steps, down 128
  }
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  for (int i = 0; i < TABLE_SIZE; i++) {   // one sine period, 0..255 around 127.5
    sineTable[i] = (uint8_t)round(127.5 * (1.0 + sin(2.0 * PI * i / TABLE_SIZE)));
  }
  readPots();
  Serial.println("Waveform: SINE");
}

void loop() {
  unsigned long now = micros();

  // 1) Button: next waveform
  if (digitalRead(BUTTON_PIN) == LOW && millis() - lastPressMs > LOCKOUT_MS) {
    lastPressMs = millis();
    waveform = (Waveform)((waveform + 1) % 3);   // SINE -> SQUARE -> TRIANGLE -> SINE
    waveIndex = 0;
    Serial.print("Waveform: ");
    Serial.println(WAVE_NAMES[waveform]);
  }

  // 2) Pots
  if (now - lastPotReadUs >= POT_READ_US) {
    lastPotReadUs = now;
    readPots();
  }

  // 3) Next sample, scaled by the amplitude
  if (now - lastStepUs >= stepUs) {
    lastStepUs = now;
    dacWrite(DAC_PIN, sampleAt(waveIndex) * amplitude / 255);
    waveIndex++;                    // 255 + 1 wraps to 0: next period
  }
}
