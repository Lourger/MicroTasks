#include <Arduino.h>
#include <EEPROM.h>

// ---------- Hardware ----------
#define BUTTON_INC_PIN   15          // increment button to GND (internal pull-up)
#define BUTTON_DEC_PIN   16          // decrement button to GND (internal pull-up)

// ---------- Storage ----------
#define EEPROM_SIZE      4           // one uint32_t
#define COUNT_ADDRESS    0
#define ERASED_VALUE     0xFFFFFFFF  // what 4 never-written bytes read as

// ---------- Timing ----------
#define DEBOUNCE_MS      50

uint32_t peopleCount = 0;

// ---------- Debounce state, one slot per button: [0] = INC, [1] = DEC ----------
const int BUTTON_PINS[2] = {BUTTON_INC_PIN, BUTTON_DEC_PIN};
int lastReading[2] = {HIGH, HIGH};
int stableLevel[2] = {HIGH, HIGH};
unsigned long lastChangeTime[2] = {0, 0};

bool buttonPressed(int index) {      // true once per press of button [index]
  int reading = digitalRead(BUTTON_PINS[index]);
  if (reading != lastReading[index]) {
    lastReading[index]    = reading;
    lastChangeTime[index] = millis();
  }
  if ((millis() - lastChangeTime[index]) >= DEBOUNCE_MS && reading != stableLevel[index]) {
    stableLevel[index] = reading;
    if (stableLevel[index] == LOW) {
      return true;
    }
  }
  return false;
}

void saveCount() {
  EEPROM.put(COUNT_ADDRESS, peopleCount);
  EEPROM.commit();
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_INC_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DEC_PIN, INPUT_PULLUP);
  EEPROM.begin(EEPROM_SIZE);

  EEPROM.get(COUNT_ADDRESS, peopleCount);
  if (peopleCount == ERASED_VALUE) {   // first run: nothing saved yet
    peopleCount = 0;
    saveCount();
  }

  Serial.println("People counter started.");
  Serial.print("Count: ");
  Serial.println(peopleCount);
}

void loop() {
  if (buttonPressed(0)) {
    peopleCount++;
    saveCount();
    Serial.print("Person in. Count: ");
    Serial.println(peopleCount);
  }

  if (buttonPressed(1)) {
    if (peopleCount > 0) {             // unsigned: never go below 0
      peopleCount--;
      saveCount();
      Serial.print("Person out. Count: ");
      Serial.println(peopleCount);
    }
  }
}
