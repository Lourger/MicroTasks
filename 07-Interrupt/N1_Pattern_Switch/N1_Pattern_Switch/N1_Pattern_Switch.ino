#include <Arduino.h>

// ---------- Pins ----------
#define BUTTON_PIN      13          // button to GND (internal pull-up)
const int LED_PINS[4] = {15, 16, 17, 18};   // each LED + 150 ohm resistor to GND

// ---------- Timing (timers count in microseconds) ----------
#define TIMER_HZ        1000000     // 1 MHz: 1 count = 1 us
#define TICK_US         10000       // pattern tick every 10 ms
#define TICKS_PER_FRAME 100         // 100 x 10 ms = 1 s per frame
#define DEBOUNCE_US     20000       // 20 ms

// ---------- Patterns: [pattern][frame][LED], 1 = on ----------
const uint8_t PATTERNS[2][2][4] = {
  {{1, 1, 1, 1}, {0, 0, 0, 0}},     // pattern 0: all on, all off
  {{1, 0, 1, 0}, {0, 1, 0, 1}}      // pattern 1: alternate
};

hw_timer_t *tickTimer = NULL;
hw_timer_t *debounceTimer = NULL;

volatile uint8_t currentPattern = 0;  // changed by the debounce ISR, used by the tick ISR
volatile bool    switchNow      = false;  // true: show the new pattern at the next tick
volatile bool    debounceReady  = true;   // false while the 20 ms debounce timer runs
uint8_t frame     = 0;              // 0 or 1; used only inside the tick ISR
uint8_t tickCount = 0;              // used only inside the tick ISR

void IRAM_ATTR showFrame(uint8_t pattern, uint8_t f) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(LED_PINS[i], PATTERNS[pattern][f][i]);
  }
}

// 1) Button pressed (falling edge): start the debounce timer once, ignore the bounces
void IRAM_ATTR onButtonFalling() {
  if (debounceReady) {
    debounceReady = false;
    timerRestart(debounceTimer);    // count from 0
    timerStart(debounceTimer);
  }
}

// 2) 20 ms later: is the button still pressed?
void IRAM_ATTR onDebounceTimer() {
  timerStop(debounceTimer);
  if (digitalRead(BUTTON_PIN) == LOW) {
    currentPattern = 1 - currentPattern;   // 0 -> 1 -> 0
    switchNow = true;
  }
  debounceReady = true;
}

// 3) Every 10 ms: next frame after 100 ticks, or at once after a switch
void IRAM_ATTR onPatternTick() {
  tickCount++;
  if (tickCount >= TICKS_PER_FRAME || switchNow) {
    showFrame(currentPattern, frame);
    frame = 1 - frame;
    switchNow = false;
    tickCount = 0;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  for (int i = 0; i < 4; i++) {
    pinMode(LED_PINS[i], OUTPUT);
  }

  debounceTimer = timerBegin(TIMER_HZ);              // starts running at once
  timerAttachInterrupt(debounceTimer, &onDebounceTimer);
  timerAlarm(debounceTimer, DEBOUNCE_US, true, 0);   // alarm after 20 ms
  timerStop(debounceTimer);                          // idle until a press

  tickTimer = timerBegin(TIMER_HZ);
  timerAttachInterrupt(tickTimer, &onPatternTick);
  timerAlarm(tickTimer, TICK_US, true, 0);           // every 10 ms, forever

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), onButtonFalling, FALLING);
}

void loop() {
  // DEMO stand-in for the task's "busy main loop": it keeps the CPU busy for 1 s
  // at a time and never reads the button. The pattern still switches at once,
  // because the interrupts do that work.
  unsigned long start = millis();
  while (millis() - start < 1000) {
    // busy: no delay(), no button check
  }
  Serial.printf("loop() is busy. Pattern: %d\n", currentPattern);
}
