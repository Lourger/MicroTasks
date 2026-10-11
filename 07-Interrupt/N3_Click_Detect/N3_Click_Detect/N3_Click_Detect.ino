#include <Arduino.h>

// ---------- Pins ----------
#define BUTTON_PIN    27            // button to GND (internal pull-up)
#define LED1_PIN      5             // single press toggles LED1 (150 ohm)
#define LED2_PIN      18            // double press toggles LED2 (150 ohm)

// ---------- Timing (timers count in microseconds) ----------
#define TIMER_HZ      1000000       // 1 MHz: 1 count = 1 us
#define WINDOW_US     500000        // click window tick: every 500 ms
#define DEBOUNCE_US   50000         // 50 ms

hw_timer_t *windowTimer = NULL;     // 500 ms ticks while a click sequence is judged
hw_timer_t *debounceTimer = NULL;   // one 50 ms check per press

volatile bool    windowRunning   = false;
volatile bool    debounceRunning = false;
volatile uint8_t clickCount      = 0;   // debounced presses in this window
volatile uint8_t windowTicks     = 0;   // 500 ms ticks since the first press
volatile bool    led1On          = false;
volatile bool    led2On          = false;

// 1) Each press (falling edge): open the window on the first press, start a debounce check
void IRAM_ATTR onButtonFalling() {
  if (!windowRunning && clickCount == 0) {
    timerRestart(windowTimer);
    timerStart(windowTimer);
    windowRunning = true;
  }
  if (!debounceRunning) {
    timerRestart(debounceTimer);
    timerStart(debounceTimer);
    debounceRunning = true;
  }
}

// 2) 50 ms after a press: still pressed? Then it was a real click
void IRAM_ATTR onDebounceTimer() {
  if (digitalRead(BUTTON_PIN) == LOW && windowRunning) {
    clickCount = clickCount + 1;
  }
  timerStop(debounceTimer);
  debounceRunning = false;
}

// 3) Every 500 ms while the window is open: decide what the clicks meant
void IRAM_ATTR onWindowTick() {
  windowTicks = windowTicks + 1;
  bool released = digitalRead(BUTTON_PIN) == HIGH;

  if (windowTicks == 1) {                         // 500 ms after the first press
    if (clickCount > 1) {                         // two or more clicks: double press
      led2On = !led2On;
      digitalWrite(LED2_PIN, led2On);
      windowTicks = 3;
    } else if (released && clickCount == 1) {     // one click, already released: single
      led1On = !led1On;
      digitalWrite(LED1_PIN, led1On);
      windowTicks = 3;
    }                                             // still held: decide at the next tick
  } else if (windowTicks == 2) {                  // 1000 ms after the first press
    if (!released && clickCount == 1) {           // held the whole time: long press
      led1On = false;
      led2On = false;
      digitalWrite(LED1_PIN, LOW);
      digitalWrite(LED2_PIN, LOW);
    }
    windowTicks = 3;
  }

  if (windowTicks >= 3) {                         // decided: close the window
    timerStop(windowTimer);
    windowRunning = false;
    clickCount = 0;
    windowTicks = 0;
  }
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);

  windowTimer = timerBegin(TIMER_HZ);              // starts running at once
  timerAttachInterrupt(windowTimer, &onWindowTick);
  timerAlarm(windowTimer, WINDOW_US, true, 0);     // every 500 ms while running
  timerStop(windowTimer);                          // idle until the first press

  debounceTimer = timerBegin(TIMER_HZ);
  timerAttachInterrupt(debounceTimer, &onDebounceTimer);
  timerAlarm(debounceTimer, DEBOUNCE_US, true, 0);
  timerStop(debounceTimer);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), onButtonFalling, FALLING);
}

void loop() {
  while (true) {
    // The task's "busy with critical tasks" main loop: the button is never read here.
  }
}
