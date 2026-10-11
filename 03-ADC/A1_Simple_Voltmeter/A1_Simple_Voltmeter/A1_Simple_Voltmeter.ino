#include <Arduino.h>

// ---------- Hardware ----------
#define VOLT_PIN       34          // potentiometer wiper (ends to 3.3 V and GND)

// ---------- ADC settings ----------
#define ADC_MAX        4095.0f     // 12-bit ADC full scale
#define V_REF          3.3f        // the voltage that full scale stands for (11 dB range)
#define READ_EVERY_MS  2000

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);              // results 0..4095
  analogSetAttenuation(ADC_11db);        // input range about 0..3.3 V
}

void loop() {
  int raw = analogRead(VOLT_PIN);                 // 0..4095
  float voltage = (raw / ADC_MAX) * V_REF;        // counts -> volts

  Serial.print("Voltage: ");
  Serial.print(voltage, 3);                       // 3 decimal places
  Serial.println(" V");

  delay(READ_EVERY_MS);
}
