#include <Arduino.h>

// ---------- UART link to Microcontroller 1 ----------
#define LINK_RX_PIN     16          // to MCU1's TX (GPIO17)
#define LINK_TX_PIN     17          // to MCU1's RX (GPIO16)
#define LINK_BAUD       115200
#define REPLY_TIMEOUT_MS 50         // don't wait forever for MCU1

// ---------- Pins ----------
#define BUTTON_PIN      15          // button to GND (internal pull-up)
#define LED2_PIN        4           // LED2 + 150 ohm resistor to GND

// ---------- Timing and PWM ----------
#define POLL_EVERY_MS   100
#define DEBOUNCE_MS     50
#define PWM_FREQ_HZ     1000
#define PWM_BITS        12          // duty 0..4095
#define PWM_MAX         4095
#define CMD_TOGGLE      'T'

unsigned long lastPoll = 0;

int lastReading = HIGH;
int stableLevel = HIGH;
unsigned long lastChangeTime = 0;

bool buttonPressed() {              // true once per press
  int reading = digitalRead(BUTTON_PIN);
  if (reading != lastReading) {
    lastReading = reading;
    lastChangeTime = millis();
  }
  if ((millis() - lastChangeTime) >= DEBOUNCE_MS && reading != stableLevel) {
    stableLevel = reading;
    if (stableLevel == LOW) {
      return true;
    }
  }
  return false;
}

// Sends one command byte and waits for one reply byte. -1 = no reply in time.
int request(uint8_t command) {
  Serial2.write(command);
  unsigned long start = millis();
  while (Serial2.available() == 0) {
    if (millis() - start >= REPLY_TIMEOUT_MS) {
      return -1;
    }
  }
  return Serial2.read();
}

void pollAdc() {
  while (Serial2.available() > 0) {  // drop anything left over
    Serial2.read();
  }
  int reply[5];
  for (uint8_t i = 0; i < 5; i++) {  // commands 0..4, one reply each
    reply[i] = request(i);
    if (reply[i] < 0) {
      Serial.println("No reply from MCU1");
      return;
    }
    delay(1);
  }
  uint16_t adcMax   = (reply[2] << 8) | reply[1];   // reply[0] is the acknowledge
  uint16_t adcValue = (reply[4] << 8) | reply[3];
  Serial.print("Received ADC Value: ");
  Serial.println(adcValue);
  if (adcMax > 0) {
    ledcWrite(LED2_PIN, map(adcValue, 0, adcMax, 0, PWM_MAX));
  }
}

void setup() {
  Serial.begin(115200);             // USB: Serial Monitor
  Serial2.begin(LINK_BAUD, SERIAL_8N1, LINK_RX_PIN, LINK_TX_PIN);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  if (!ledcAttach(LED2_PIN, PWM_FREQ_HZ, PWM_BITS)) {
    Serial.println("Error: ledcAttach failed");
  }
}

void loop() {
  if (buttonPressed()) {
    Serial2.write(CMD_TOGGLE);      // MCU1 toggles LED1
  }
  if (millis() - lastPoll >= POLL_EVERY_MS) {
    lastPoll = millis();
    pollAdc();
  }
}
