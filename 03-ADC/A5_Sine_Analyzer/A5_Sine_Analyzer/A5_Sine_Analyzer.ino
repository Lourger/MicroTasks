#include <Arduino.h>

// ---------- Hardware (one board, loop-back) ----------
#define DAC_PIN         25          // DAC1 output: the sine wave
#define ADC_PIN         34          // ADC input: jumper wire from GPIO25 to GPIO34

// ---------- Sine settings ----------
#define DAC_OFFSET      128         // middle of 0..255 -> about 1.65 V
#define DAC_AMPLITUDE   64          // +-64 counts -> about +-0.83 V
#define MEASURE_MS      1000        // collect samples for 1 s per result
#define PRINT_PAUSE_MS  100

int angleDeg = 0;                   // where on the sine wave we are (0..359)

void outputNextSinePoint() {                          // one point per call, 360 points per cycle
  float radians = angleDeg * PI / 180.0f;
  int dacValue = DAC_OFFSET + (int)(DAC_AMPLITUDE * sin(radians));
  dacWrite(DAC_PIN, dacValue);
  angleDeg = (angleDeg + 1) % 360;
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
}

void loop() {
  // 1) For 1 s: output the next sine point, then sample it, and keep the lowest and highest values
  uint32_t minMv = 0xFFFFFFFF;
  uint32_t maxMv = 0;
  unsigned long start = millis();
  while (millis() - start < MEASURE_MS) {
    outputNextSinePoint();
    uint32_t mv = analogReadMilliVolts(ADC_PIN);
    if (mv < minMv) minMv = mv;
    if (mv > maxMv) maxMv = mv;
  }

  // 2) Peak from the swing, then RMS and average from the peak (sine wave formulas)
  float peakV    = (maxMv - minMv) / 2.0f / 1000.0f;
  float rmsV     = peakV / sqrt(2.0f);
  float averageV = 0.637f * peakV;

  Serial.print("Peak: ");
  Serial.print(peakV, 3);
  Serial.print("\tRMS: ");
  Serial.print(rmsV, 3);
  Serial.print("\tAverage: ");
  Serial.println(averageV, 3);

  delay(PRINT_PAUSE_MS);
}
