#include <Arduino.h>

// ---------- UART link to Microcontroller 2 ----------
#define LINK_RX_PIN   16            // to MCU2's TX (GPIO17)
#define LINK_TX_PIN   17            // to MCU2's RX (GPIO16)
#define LINK_BAUD     115200

// ---------- Pins ----------
#define LED1_PIN      4             // LED1 + 150 ohm resistor to GND
#define POT_PIN       34            // potentiometer wiper (ADC1)

// ---------- Protocol: one-byte commands from MCU2 ----------
#define CMD_TOGGLE    'T'           // toggle LED1
// commands 0..4 (raw byte values, not the characters '0'..'4'):
//   0 = read the pot now, reply 0 (acknowledge)
//   1, 2 = reply ADC full scale, low byte / high byte
//   3, 4 = reply the reading,    low byte / high byte
#define ADC_MAX       4095

uint16_t lastReading = 0;
bool led1On = false;

void setup() {
  Serial.begin(115200);             // USB: Serial Monitor
  Serial2.begin(LINK_BAUD, SERIAL_8N1, LINK_RX_PIN, LINK_TX_PIN);
  pinMode(LED1_PIN, OUTPUT);
  analogReadResolution(12);
}

void loop() {
  if (Serial2.available() > 0) {
    uint8_t command = Serial2.read();

    if (command == CMD_TOGGLE) {
      led1On = !led1On;
      digitalWrite(LED1_PIN, led1On ? HIGH : LOW);
      Serial.print("LED is ");
      Serial.println(led1On ? "ON" : "OFF");
    } else if (command == 0) {
      lastReading = analogRead(POT_PIN);
      Serial2.write((uint8_t)0);                    // acknowledge
    } else if (command == 1) {
      Serial2.write((uint8_t)(ADC_MAX & 0xFF));
    } else if (command == 2) {
      Serial2.write((uint8_t)(ADC_MAX >> 8));
    } else if (command == 3) {
      Serial2.write((uint8_t)(lastReading & 0xFF));
    } else if (command == 4) {
      Serial2.write((uint8_t)(lastReading >> 8));
    }
  }
}
