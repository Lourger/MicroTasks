#include <Arduino.h>
#include "driver/mcpwm.h"          // ESP32 motor-control PWM (MCPWM), legacy driver

// ---------- Hardware ----------
#define PWM_PIN        14          // GPIO14 -> motor driver input (MOSFET gate through 220 ohm)
#define POT_PIN        34          // potentiometer wiper

// ---------- PWM settings ----------
#define PWM_FREQ_HZ    10000       // 10 kHz: inside the motor's 5..10 kHz window
#define ADC_MAX        4095.0f     // 12-bit ADC full scale

void setup() {
  analogReadResolution(12);

  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, PWM_PIN);       // route MCPWM unit 0, output 0A to GPIO14

  mcpwm_config_t pwmConfig = {};
  pwmConfig.frequency    = PWM_FREQ_HZ;
  pwmConfig.cmpr_a       = 0;                            // output A duty in %, start at 0 %
  pwmConfig.cmpr_b       = 0;                            // output B is not used
  pwmConfig.counter_mode = MCPWM_UP_COUNTER;
  pwmConfig.duty_mode    = MCPWM_DUTY_MODE_0;            // active high: duty = time the pin is HIGH
  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwmConfig);
}

void loop() {
  int potValue = analogRead(POT_PIN);                    // 0..4095
  float dutyPercent = (potValue / ADC_MAX) * 100.0f;     // 0.0 .. 100.0 %

  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, dutyPercent);
  mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
  delay(10);
}
