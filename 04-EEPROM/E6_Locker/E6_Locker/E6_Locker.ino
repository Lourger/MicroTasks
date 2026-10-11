#include <Arduino.h>
#include <EEPROM.h>

// ---------- Storage ----------
#define EEPROM_SIZE    32
#define PIN_ADDRESS    0             // the user PIN: 4 ASCII digits at address 0..3
#define PIN_LENGTH     4
#define ERASED_BYTE    0xFF          // address 0 still erased = no PIN set yet

// ---------- Master PIN (fixed in the firmware) ----------
const char MASTER_PIN[PIN_LENGTH + 1] = "1234";

String readLine() {                  // waits for a full line, returns it without spaces/newline
  while (Serial.available() == 0) {
  }
  String line = Serial.readStringUntil('\n');
  line.trim();
  return line;
}

bool isValidPin(const String& pin) { // exactly 4 digits
  if (pin.length() != PIN_LENGTH) {
    return false;
  }
  for (int i = 0; i < PIN_LENGTH; i++) {
    if (!isDigit(pin[i])) {
      return false;
    }
  }
  return true;
}

void saveUserPin(const String& pin) {
  for (int i = 0; i < PIN_LENGTH; i++) {
    EEPROM.write(PIN_ADDRESS + i, pin[i]);
  }
  EEPROM.commit();
}

bool matchesUserPin(const String& pin) {
  if (pin.length() != PIN_LENGTH) {  // "12345" must not unlock a "1234" locker
    return false;
  }
  for (int i = 0; i < PIN_LENGTH; i++) {
    if (EEPROM.read(PIN_ADDRESS + i) != pin[i]) {
      return false;
    }
  }
  return true;
}

String askNewPin() {                 // keeps asking until a valid 4-digit PIN is typed
  while (true) {
    Serial.println("Enter a new 4-digit PIN:");
    String pin = readLine();
    if (isValidPin(pin)) {
      return pin;
    }
    Serial.println("A PIN must be exactly 4 digits.");
  }
}

void showMenu() {
  Serial.println();
  Serial.println("1 Unlock   2 Change PIN   3 Reset PIN with master PIN");
  Serial.println("Choose 1, 2 or 3:");
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);
  Serial.println("=== Locker ===");

  if (EEPROM.read(PIN_ADDRESS) == ERASED_BYTE) {   // first use
    Serial.println("No PIN set yet.");
    saveUserPin(askNewPin());
    Serial.println("PIN saved.");
  } else {
    Serial.println("A PIN is already set.");
  }
  showMenu();
}

void loop() {
  if (Serial.available() == 0) {
    return;
  }

  String choice = readLine();
  if (choice == "1") {
    Serial.println("Enter PIN:");
    if (matchesUserPin(readLine())) {
      Serial.println("Access granted. Locker open.");
    } else {
      Serial.println("Access denied.");
    }
  } else if (choice == "2") {
    Serial.println("Enter current PIN:");
    if (matchesUserPin(readLine())) {
      saveUserPin(askNewPin());
      Serial.println("PIN changed.");
    } else {
      Serial.println("Wrong PIN. Not changed.");
    }
  } else if (choice == "3") {
    Serial.println("Enter master PIN:");
    if (readLine() == MASTER_PIN) {
      String pin = askNewPin();
      saveUserPin(pin);
      Serial.print("PIN reset. New PIN stored: ");
      Serial.println(pin);
    } else {
      Serial.println("Wrong master PIN.");
    }
  } else {
    Serial.println("Invalid choice.");
  }
  showMenu();
}
