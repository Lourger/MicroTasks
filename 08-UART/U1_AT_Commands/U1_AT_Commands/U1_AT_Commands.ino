#include <Arduino.h>

// ---------- Pins ----------
#define LED_PIN   13                // LED + 150 ohm resistor to GND
#define POT_PIN   34                // potentiometer wiper (ADC1)

// ---------- ADC ----------
#define ADC_MAX   4095.0f           // 12-bit full scale
#define V_REF     3.3f

void reportUptime() {               // +SYS_ON_TIME:"HH:MM:SS"
  unsigned long totalSeconds = millis() / 1000;
  unsigned long hours   = totalSeconds / 3600;
  unsigned long minutes = (totalSeconds / 60) % 60;
  unsigned long seconds = totalSeconds % 60;
  Serial.printf("+SYS_ON_TIME:\"%02lu:%02lu:%02lu\"\n", hours, minutes, seconds);
}

void handleCommand(const String &command) {
  if (command == "AT") {
    Serial.println("OK");
  } else if (command == "AT+LED=ON") {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("+LED:\"ON\"");
  } else if (command == "AT+LED=OFF") {
    digitalWrite(LED_PIN, LOW);
    Serial.println("+LED:\"OFF\"");
  } else if (command == "AT+LED_STATUS?") {
    bool on = digitalRead(LED_PIN) == HIGH;   // reads the level the pin is driving
    Serial.println(on ? "+LED_STATUS:\"ON\"" : "+LED_STATUS:\"OFF\"");
  } else if (command == "AT+ADC_VALUE?") {
    Serial.print("+ADC_VALUE:");
    Serial.println(analogRead(POT_PIN));       // 0..4095
  } else if (command == "AT+ADC_VOLTAGE?") {
    float volts = analogRead(POT_PIN) * V_REF / ADC_MAX;
    Serial.print("+ADC_VOLTAGE:");
    Serial.println(volts, 2);                  // 0.00..3.30
  } else if (command == "AT+SYS_ON_TIME?") {
    reportUptime();
  } else {
    Serial.println("Invalid Command!");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  analogReadResolution(12);
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');   // one line, up to the Enter key
    command.trim();                                  // drop '\r', spaces
    if (command.length() == 0) {
      return;
    }
    Serial.print("Received: ");
    Serial.println(command);
    handleCommand(command);
  }
}
