#include <Arduino.h>
#include <driver/ledc.h>

// ---------- Outputs ----------
#define WAVE1_PIN      4            // 307 kHz
#define WAVE2_PIN      5            // 570 kHz
#define WAVE1_FREQ_HZ  307000
#define WAVE2_FREQ_HZ  570000

// ---------- LEDC settings ----------
#define LEDC_BITS      LEDC_TIMER_2_BIT   // duty 0..3
#define DUTY_50        2                  // 2 of 4 = 50 %

// One LEDC timer (sets the frequency) and one channel (drives the pin) per wave
void startWave(ledc_timer_t timer, ledc_channel_t channel, int pin, uint32_t freqHz) {
  ledc_timer_config_t timerConfig = {};
  timerConfig.speed_mode      = LEDC_HIGH_SPEED_MODE;
  timerConfig.duty_resolution = LEDC_BITS;
  timerConfig.timer_num       = timer;
  timerConfig.freq_hz         = freqHz;
  timerConfig.clk_cfg         = LEDC_AUTO_CLK;
  ledc_timer_config(&timerConfig);

  ledc_channel_config_t channelConfig = {};
  channelConfig.gpio_num   = pin;
  channelConfig.speed_mode = LEDC_HIGH_SPEED_MODE;
  channelConfig.channel    = channel;
  channelConfig.intr_type  = LEDC_INTR_DISABLE;
  channelConfig.timer_sel  = timer;
  channelConfig.duty       = DUTY_50;
  channelConfig.hpoint     = 0;
  ledc_channel_config(&channelConfig);
}

void setup() {
  startWave(LEDC_TIMER_0, LEDC_CHANNEL_0, WAVE1_PIN, WAVE1_FREQ_HZ);
  startWave(LEDC_TIMER_1, LEDC_CHANNEL_1, WAVE2_PIN, WAVE2_FREQ_HZ);
}

void loop() {
  // free: the LEDC hardware makes both waves on its own
}
