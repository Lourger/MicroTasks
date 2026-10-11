#include <Arduino.h>
#include <driver/pulse_cnt.h>

// ---------- Pins ----------
#define PULSE_PIN     4             // pulses in (3.3 V logic, common GND)
#define BUTTON_PIN    14            // button to GND (internal pull-up)
#define DEBOUNCE_MS   50

// ---------- Pulse counter (PCNT) ----------
#define PCNT_HIGH_LIMIT 32767       // the hardware counter is 16-bit signed
#define PCNT_LOW_LIMIT  -1          // must be below 0; never reached (we only count up)

pcnt_unit_handle_t pcntUnit = NULL;
volatile uint32_t overflowCount = 0;   // times the counter reached 32767 and reset to 0

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

// Runs (in an interrupt) each time the counter reaches 32767; the hardware then resets it to 0
bool IRAM_ATTR onCounterLimit(pcnt_unit_handle_t unit, const pcnt_watch_event_data_t *event, void *ctx) {
  overflowCount = overflowCount + 1;
  return false;                     // no task needs waking
}

void setupPulseCounter() {
  pcnt_unit_config_t unitConfig = {};
  unitConfig.high_limit = PCNT_HIGH_LIMIT;
  unitConfig.low_limit  = PCNT_LOW_LIMIT;
  pcnt_new_unit(&unitConfig, &pcntUnit);

  pcnt_chan_config_t channelConfig = {};
  channelConfig.edge_gpio_num  = PULSE_PIN;
  channelConfig.level_gpio_num = -1;  // no control pin
  pcnt_channel_handle_t channel = NULL;
  pcnt_new_channel(pcntUnit, &channelConfig, &channel);
  pcnt_channel_set_edge_action(channel, PCNT_CHANNEL_EDGE_ACTION_INCREASE,  // rising edge: +1
                                        PCNT_CHANNEL_EDGE_ACTION_HOLD);     // falling edge: ignore
  pcnt_channel_set_level_action(channel, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_KEEP);

  pcnt_unit_add_watch_point(pcntUnit, PCNT_HIGH_LIMIT);
  pcnt_event_callbacks_t callbacks = {};
  callbacks.on_reach = onCounterLimit;
  pcnt_unit_register_event_callbacks(pcntUnit, &callbacks, NULL);

  pcnt_unit_enable(pcntUnit);
  pcnt_unit_clear_count(pcntUnit);
  pcnt_unit_start(pcntUnit);        // from now on the hardware counts by itself
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  setupPulseCounter();
  Serial.println("Counting pulses on GPIO4. Press the button to show and reset the count.");
}

void loop() {
  if (buttonPressed()) {
    int count = 0;
    pcnt_unit_get_count(pcntUnit, &count);
    uint32_t totalPulses = overflowCount * (uint32_t)PCNT_HIGH_LIMIT + (uint32_t)count;
    Serial.print("Number of Pulses: ");
    Serial.println(totalPulses);

    pcnt_unit_clear_count(pcntUnit); // start again from 0
    overflowCount = 0;
  }
}
