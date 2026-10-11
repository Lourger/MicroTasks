#include <Arduino.h>

// ---------- Hardware ----------
// Each known resistor goes from its drive pin to the MEASURE node.
// The unknown resistor goes from the MEASURE node to GND.
#define MEASURE_PIN     34                     // ADC at the divider middle
#define RANGE_COUNT     4
const int   DRIVE_PINS[RANGE_COUNT]  = {25, 26, 27, 32};
const float KNOWN_OHMS[RANGE_COUNT]  = {220.0f, 1200.0f, 12000.0f, 47000.0f};   // put your measured values here

// ---------- Measurement settings ----------
#define DRIVE_MV        3300.0f                // drive pin HIGH level; measure yours with a multimeter
#define MID_MV          1650.0f                // best accuracy when the middle sits near half of DRIVE_MV
#define OPEN_MV         3100.0f                // at or above this on every range: nothing connected
#define SETTLE_MS       10
#define MEASURE_EVERY_MS 500

void releaseAllDrivePins() {
  for (int i = 0; i < RANGE_COUNT; i++) {
    pinMode(DRIVE_PINS[i], INPUT);             // Hi-Z: this resistor is out of the circuit
  }
}

uint32_t measureRange(int range) {             // returns the middle voltage in mV for one range
  releaseAllDrivePins();
  pinMode(DRIVE_PINS[range], OUTPUT);
  digitalWrite(DRIVE_PINS[range], HIGH);       // only this known resistor feeds the divider
  delay(SETTLE_MS);
  return analogReadMilliVolts(MEASURE_PIN);
}

void printResistance(float ohms) {
  if (ohms >= 1000000.0f) {
    return;                                    // 1 Mohm or more: out of range, not printed
  }
  Serial.print("Resistance: ");
  if (ohms < 1000.0f) {
    Serial.print(ohms, 3);
    Serial.println(" ohm");
  } else {
    Serial.print(ohms / 1000.0f, 3);
    Serial.println(" kohm");
  }
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  releaseAllDrivePins();
  Serial.println("Smart Ohmmeter ready");
}

void loop() {
  int   bestRange = -1;
  float bestError = 1e9f;
  uint32_t bestMv = 0;

  // 1) Try every range and keep the one whose middle voltage is closest to MID_MV
  for (int range = 0; range < RANGE_COUNT; range++) {
    uint32_t mv = measureRange(range);
    float error = fabsf(mv - MID_MV);
    if (mv < OPEN_MV && error < bestError) {
      bestError = error;
      bestRange = range;
      bestMv    = mv;
    }
  }
  releaseAllDrivePins();

  // 2) Divider formula on the chosen range: Rx = Rknown x V / (Vdrive - V)
  if (bestRange >= 0) {
    float rx = KNOWN_OHMS[bestRange] * bestMv / (DRIVE_MV - bestMv);
    printResistance(rx);
  }                                            // no range below OPEN_MV: nothing connected, nothing printed

  delay(MEASURE_EVERY_MS);
}
