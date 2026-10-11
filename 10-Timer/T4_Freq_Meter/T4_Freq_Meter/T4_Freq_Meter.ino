#include <Arduino.h>
#include <driver/pulse_cnt.h>

// ---------- Input ----------
#define SIGNAL_PIN      4           // square wave in (3.3 V logic, common GND)

// ---------- Measurement ----------
#define GATE_MS         10          // count for 10 ms
#define REPEAT_MS       1000        // one measurement per second
#define PCNT_HIGH_LIMIT 32767
#define PCNT_LOW_LIMIT  -32768

pcnt_unit_handle_t pcntUnit = NULL;

void setupPulseCounter() {
  pcnt_unit_config_t unitConfig = {};
  unitConfig.high_limit = PCNT_HIGH_LIMIT;
  unitConfig.low_limit  = PCNT_LOW_LIMIT;
  unitConfig.flags.accum_count = 1; // the driver adds up overflows past 32767
  pcnt_new_unit(&unitConfig, &pcntUnit);

  pcnt_chan_config_t channelConfig = {};
  channelConfig.edge_gpio_num  = SIGNAL_PIN;
  channelConfig.level_gpio_num = -1;
  pcnt_channel_handle_t channel = NULL;
  pcnt_new_channel(pcntUnit, &channelConfig, &channel);
  pcnt_channel_set_edge_action(channel, PCNT_CHANNEL_EDGE_ACTION_INCREASE,  // rising: +1
                                        PCNT_CHANNEL_EDGE_ACTION_HOLD);     // falling: ignore
  pcnt_channel_set_level_action(channel, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_KEEP);

  pcnt_unit_add_watch_point(pcntUnit, PCNT_HIGH_LIMIT);   // needed for accum_count
  pcnt_unit_enable(pcntUnit);
}

void setup() {
  Serial.begin(115200);
  setupPulseCounter();
  Serial.println("Frequency Measurement");
  Serial.println("PCNT initialized on GPIO4.");
}

void loop() {
  // 1) Count rising edges for about GATE_MS, and time the window exactly
  pcnt_unit_clear_count(pcntUnit);
  int64_t startUs = esp_timer_get_time();
  pcnt_unit_start(pcntUnit);
  delay(GATE_MS);
  pcnt_unit_stop(pcntUnit);
  int64_t gateUs = esp_timer_get_time() - startUs;

  int count = 0;
  pcnt_unit_get_count(pcntUnit, &count);

  // 2) Frequency = edges / seconds
  double frequencyHz = count * 1000000.0 / gateUs;
  Serial.printf("Frequency: %.2f Hz\n", frequencyHz);

  delay(REPEAT_MS);
}
