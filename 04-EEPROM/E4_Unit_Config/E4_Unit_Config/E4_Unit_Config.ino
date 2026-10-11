#include <Arduino.h>
#include <EEPROM.h>

// ---------- What is stored: 4 bytes, one per preference ----------
struct UnitPreferences {
  uint8_t temperature;               // 0 = Celsius,    1 = Fahrenheit
  uint8_t distance;                  // 0 = Kilometers, 1 = Miles
  uint8_t weight;                    // 0 = Kilograms,  1 = Pounds
  uint8_t volume;                    // 0 = Liters,     1 = Gallons
};

#define PREFS_ADDRESS  0
const UnitPreferences FACTORY_DEFAULTS = {0, 0, 0, 0};

const char* const NAMES[4]    = {"Temperature", "Distance", "Weight", "Volume"};
const char* const UNITS[4][2] = {{"Celsius", "Fahrenheit"}, {"Kilometers", "Miles"},
                                 {"Kilograms", "Pounds"},   {"Liters", "Gallons"}};

UnitPreferences prefs;               // the working copy in RAM

uint8_t* field(int index) {          // the address of preference 0..3 inside prefs
  uint8_t* fields[4] = {&prefs.temperature, &prefs.distance, &prefs.weight, &prefs.volume};
  return fields[index];
}

void savePreferences() {
  EEPROM.put(PREFS_ADDRESS, prefs);  // copy the struct into the RAM copy of the EEPROM
  if (EEPROM.commit()) {             // write it to flash
    Serial.println("Saved.");
  } else {
    Serial.println("Save failed!");
  }
}

void loadPreferences() {
  EEPROM.get(PREFS_ADDRESS, prefs);
  for (int i = 0; i < 4; i++) {
    if (*field(i) > 1) {             // a fresh EEPROM reads 0xFF: not a valid choice
      Serial.println("No valid settings found: restoring factory defaults.");
      prefs = FACTORY_DEFAULTS;
      savePreferences();
      return;
    }
  }
}

void showPreferences() {
  Serial.println("Current preferences:");
  for (int i = 0; i < 4; i++) {
    Serial.print("  ");
    Serial.print(NAMES[i]);
    Serial.print(": ");
    Serial.println(UNITS[i][*field(i)]);
  }
}

void showMenu() {
  Serial.println();
  Serial.println("Menu: 1 Temperature  2 Distance  3 Weight  4 Volume  5 Restore defaults");
  Serial.println("Enter an option (1-5):");
}

int readNumberLine() {               // waits for a full line and returns it as a number
  while (Serial.available() == 0) {
  }
  String line = Serial.readStringUntil('\n');
  line.trim();
  return line.toInt();               // text that is not a number gives 0
}

void setup() {
  delay(2000);
  Serial.begin(115200);
  EEPROM.begin(sizeof(UnitPreferences));
  loadPreferences();
  showPreferences();
  showMenu();
}

void loop() {
  if (Serial.available() == 0) {
    return;
  }

  int option = readNumberLine();
  if (option >= 1 && option <= 4) {
    int index = option - 1;
    Serial.print("Choose ");
    Serial.print(NAMES[index]);
    Serial.print(": 0 = ");
    Serial.print(UNITS[index][0]);
    Serial.print(", 1 = ");
    Serial.println(UNITS[index][1]);

    int value = readNumberLine();
    if (value == 0 || value == 1) {
      *field(index) = value;
      savePreferences();
    } else {
      Serial.println("Invalid value. Use 0 or 1.");
    }
  } else if (option == 5) {
    prefs = FACTORY_DEFAULTS;
    savePreferences();
  } else {
    Serial.println("Invalid option.");
  }

  showPreferences();
  showMenu();
}
