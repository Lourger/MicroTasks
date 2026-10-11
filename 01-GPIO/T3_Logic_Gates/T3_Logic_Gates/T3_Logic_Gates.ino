#include <Arduino.h>

// ---------- Hardware ----------
#define SWITCH_A_PIN   14     // switch A between GPIO14 and GND (internal pull-up)
#define SWITCH_B_PIN   13     // switch B between GPIO13 and GND (internal pull-up)

#define LED_OR_PIN     15     // each LED: GPIO -> 150 ohm -> LED -> GND
#define LED_AND_PIN    16
#define LED_NOR_PIN    17
#define LED_NAND_PIN   18

// ---------- Last inputs seen (so the LEDs update only on a change) ----------
int previousA = -1;           // -1 = nothing read yet, so the first pass always updates
int previousB = -1;

void setup() {
  pinMode(SWITCH_A_PIN, INPUT_PULLUP);
  pinMode(SWITCH_B_PIN, INPUT_PULLUP);

  pinMode(LED_OR_PIN,   OUTPUT);
  pinMode(LED_AND_PIN,  OUTPUT);
  pinMode(LED_NOR_PIN,  OUTPUT);
  pinMode(LED_NAND_PIN, OUTPUT);
}

void loop() {
  // The pin level is the input bit: HIGH = 1 (switch open), LOW = 0 (switch to GND)
  int a = digitalRead(SWITCH_A_PIN);
  int b = digitalRead(SWITCH_B_PIN);

  if (a != previousA || b != previousB) {
    int resultOr   = a | b;          // bitwise OR of two 1-bit values
    int resultAnd  = a & b;          // bitwise AND
    int resultNor  = !resultOr;      // NOT of OR
    int resultNand = !resultAnd;     // NOT of AND

    digitalWrite(LED_OR_PIN,   resultOr);     // 1 = HIGH = LED ON
    digitalWrite(LED_AND_PIN,  resultAnd);
    digitalWrite(LED_NOR_PIN,  resultNor);
    digitalWrite(LED_NAND_PIN, resultNand);

    previousA = a;
    previousB = b;
  }
}
