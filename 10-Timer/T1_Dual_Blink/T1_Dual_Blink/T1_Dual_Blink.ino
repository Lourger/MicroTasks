#include <Arduino.h>

// ---------- Pins ----------
#define LED1_PIN      2             // LED1 + 150 ohm resistor to GND
#define LED2_PIN      4             // LED2 + 150 ohm resistor to GND

// ---------- Timing (timers count in microseconds) ----------
#define TIMER_HZ      1000000       // 1 MHz: 1 count = 1 us
#define LED1_TOGGLE_US 100000       // LED1 toggles every 100 ms
#define LED2_TOGGLE_US 360000       // LED2 toggles every 360 ms

hw_timer_t *timer1 = NULL;
hw_timer_t *timer2 = NULL;

volatile bool led1On = false;       // changed only by the ISRs
volatile bool led2On = false;

void IRAM_ATTR onTimer1() {         // every 100 ms
  led1On = !led1On;
  digitalWrite(LED1_PIN, led1On);
}

void IRAM_ATTR onTimer2() {         // every 360 ms
  led2On = !led2On;
  digitalWrite(LED2_PIN, led2On);
}

void setup() {
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);

  timer1 = timerBegin(TIMER_HZ);                   // one hardware timer per LED
  timerAttachInterrupt(timer1, &onTimer1);
  timerAlarm(timer1, LED1_TOGGLE_US, true, 0);     // auto-reload: repeats forever

  timer2 = timerBegin(TIMER_HZ);
  timerAttachInterrupt(timer2, &onTimer2);
  timerAlarm(timer2, LED2_TOGGLE_US, true, 0);
}

void loop() {
  // free for the "critical tasks": the hardware timers toggle the LEDs
}
