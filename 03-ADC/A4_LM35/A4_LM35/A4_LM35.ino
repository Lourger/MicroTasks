#include <Arduino.h>

// ---------- Hardware ----------
// LM35: +Vs -> 5 V, Vout -> GPIO34, GND pin -> divider (10 kohm from 5 V, 3.3 kohm to GND) and GPIO35
#define LM35_OUT_PIN    34          // LM35 output voltage
#define LM35_GND_PIN    35          // LM35 GND pin, lifted to about 1.24 V by the divider

// ---------- Conversion ----------
#define ADC_MAX         4095.0f     // 12-bit full scale
#define V_REF_MV        3300.0f     // full scale in millivolts
#define MV_PER_DEGREE   10.0f       // LM35: 10 mV per degree C
#define READ_EVERY_MS   1000

float readMilliVolts(int pin) {
  return analogRead(pin) * (V_REF_MV / ADC_MAX);      // counts -> mV
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
}

void loop() {
  float outMv    = readMilliVolts(LM35_OUT_PIN);
  float offsetMv = readMilliVolts(LM35_GND_PIN);
  float temperatureC = (outMv - offsetMv) / MV_PER_DEGREE;   // can be negative

  Serial.print("Temperature: ");
  Serial.print(temperatureC);                                // 2 decimal places by default
  Serial.println(" C");

  delay(READ_EVERY_MS);
}
