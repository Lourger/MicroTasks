#include <Arduino.h>
#include "driver/mcpwm_prelude.h"  // ESP32 motor-control PWM (MCPWM), current ESP-IDF 5 driver

// ---------- Hardware ----------
#define PWM_PIN          14        // GPIO14 -> motor driver input
#define POT_PIN          34        // potentiometer wiper

// ---------- PWM settings ----------
#define TIMER_RES_HZ     10000000  // timer counts at 10 MHz (one tick = 0.1 us)
#define PERIOD_TICKS     1000      // 10 MHz / 1000 ticks = 10 kHz PWM

mcpwm_cmpr_handle_t comparator = NULL;   // set in setup(), used in loop() to change the duty

void setup() {
  analogReadResolution(12);

  // 1) Timer: counts 0..999 again and again at 10 MHz -> one PWM period every 100 us
  mcpwm_timer_handle_t timer = NULL;
  mcpwm_timer_config_t timerConfig = {};
  timerConfig.group_id      = 0;
  timerConfig.clk_src       = MCPWM_TIMER_CLK_SRC_DEFAULT;
  timerConfig.resolution_hz = TIMER_RES_HZ;
  timerConfig.count_mode    = MCPWM_TIMER_COUNT_MODE_UP;
  timerConfig.period_ticks  = PERIOD_TICKS;
  mcpwm_new_timer(&timerConfig, &timer);

  // 2) Operator: links the timer to a comparator and a generator
  mcpwm_oper_handle_t oper = NULL;
  mcpwm_operator_config_t operConfig = {};
  operConfig.group_id = 0;
  mcpwm_new_operator(&operConfig, &oper);
  mcpwm_operator_connect_timer(oper, timer);

  // 3) Comparator: holds the duty as a tick count (updated at the start of each period)
  mcpwm_comparator_config_t cmprConfig = {};
  cmprConfig.flags.update_cmp_on_tez = true;
  mcpwm_new_comparator(oper, &cmprConfig, &comparator);
  mcpwm_comparator_set_compare_value(comparator, 0);

  // 4) Generator: drives the pin HIGH at the start of each period, LOW at the compare value
  mcpwm_gen_handle_t generator = NULL;
  mcpwm_generator_config_t genConfig = {};
  genConfig.gen_gpio_num = PWM_PIN;
  mcpwm_new_generator(oper, &genConfig, &generator);
  mcpwm_generator_set_action_on_timer_event(generator,
      MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH));
  mcpwm_generator_set_action_on_compare_event(generator,
      MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparator, MCPWM_GEN_ACTION_LOW));

  // 5) Start the timer
  mcpwm_timer_enable(timer);
  mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP);
}

void loop() {
  int potValue = analogRead(POT_PIN);                            // 0..4095
  uint32_t compareTicks = (uint32_t)potValue * PERIOD_TICKS / 4096; // 0..999 ticks HIGH per period
  mcpwm_comparator_set_compare_value(comparator, compareTicks);
  delay(10);
}
