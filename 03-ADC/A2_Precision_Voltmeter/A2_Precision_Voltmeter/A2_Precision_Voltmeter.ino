#include <Arduino.h>

// ---------- Hardware ----------
#define VOLT_PIN        34          // potentiometer wiper (ends to 3.3 V and GND)

// ---------- Ranges ----------
// Fine range  : 0 dB attenuation, about 0..0.95 V, about 0.23 mV per step
// Coarse range: 11 dB attenuation, about 0..3.1 V, about 0.8 mV per step
#define ADC_FULL_SCALE  4095        // a reading this high means the range is saturated
#define SWITCH_DOWN_MV  850         // in the coarse range, below this -> go back to the fine range
#define READ_EVERY_MS   500

bool fineRange = true;              // which range is active now

void selectRange(bool fine) {
  fineRange = fine;
  analogSetAttenuation(fine ? ADC_0db : ADC_11db);   // global call: also rebuilds the mV calibration
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  selectRange(true);                                  // start in the fine range
}

void loop() {
  // 1) First reading: decide whether the range must change
  int raw = analogRead(VOLT_PIN);
  if (fineRange && raw >= ADC_FULL_SCALE) {
    selectRange(false);                               // too high for 0.95 V: use the coarse range
  } else if (!fineRange && analogReadMilliVolts(VOLT_PIN) < SWITCH_DOWN_MV) {
    selectRange(true);                                // low enough: use the fine range
  }

  // 2) Second reading, in the range now active: this one is printed
  raw = analogRead(VOLT_PIN);
  uint32_t milliVolts = analogReadMilliVolts(VOLT_PIN);   // calibrated with the chip's eFuse data

  Serial.print("Raw: ");
  Serial.print(raw);
  Serial.print("  Voltage: ");
  Serial.print(milliVolts / 1000.0f, 3);
  Serial.print(" V  (");
  Serial.print(fineRange ? "fine 0-0.95 V" : "coarse 0-3.1 V");
  Serial.println(")");

  delay(READ_EVERY_MS);
}
