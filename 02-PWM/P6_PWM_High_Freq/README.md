# P6: PWM High-Frequency Generation

Drive a DC motor with 10 kHz PWM; a potentiometer sets the speed.

## The task

A DC motor needs a PWM frequency between **5 kHz and 10 kHz**; lower frequencies make it whine. Generate PWM in that range and let a potentiometer set the duty cycle, which sets the motor's speed.

`P6_PWM_High_Freq` uses the ESP32's **MCPWM** (motor-control PWM) peripheral at 10 kHz through its older (legacy) driver, `driver/mcpwm.h`. `P6_PWM_High_Freq_NewDriver` does the same with the current driver; see *Upload and run*.

## Rules

| Rule | Value |
|---|---|
| PWM | MCPWM unit 0, output 0A, 10 kHz, active high |
| Duty | pot 0..4095 → 0..100 % |
| Update | every 10 ms |
| Pins | PWM GPIO14, pot GPIO34 |

## Parts

- ESP32 DevKit
- small DC motor + its own supply (5 to 12 V)
- logic-level N-MOSFET, e.g. IRLZ44N
- Schottky diode, e.g. 1N5819
- resistors 220 Ω and 10 kΩ
- 1 potentiometer, 1 kΩ to 10 kΩ
- jumper wires

## Wiring

| ESP32 pin | Set up as | Connection (from the pin outward) |
|---|---|---|
| GPIO14 | MCPWM0A | 220 Ω resistor → MOSFET gate |
| GPIO34 | ADC input | 10 kΩ pot: middle leg (wiper); outer legs to 3.3 V and GND |

The motor runs through a low-side MOSFET driver; all its connections are in *Motor driver* below.

## Upload and run

This task has more than one version of the program. Each runs on its own on the same board and wiring; flash the one you want.

| Sketch | Board / version | Serial Monitor |
|---|---|---|
| [`P6_PWM_High_Freq`](P6_PWM_High_Freq/P6_PWM_High_Freq.ino) | Legacy MCPWM driver (`driver/mcpwm.h`) | not used |
| [`P6_PWM_High_Freq_NewDriver`](P6_PWM_High_Freq_NewDriver/P6_PWM_High_Freq_NewDriver.ino) | Current MCPWM driver (`driver/mcpwm_prelude.h`) | not used |

For every sketch: select the board **ESP32 Dev Module** in Arduino IDE 2, open the `.ino` file and upload.

`P6_PWM_High_Freq` compiles with one expected warning: *legacy MCPWM driver is deprecated, please migrate to the new driver*. The sketch still works; `P6_PWM_High_Freq_NewDriver` uses that new driver and compiles without it.

### P6_PWM_High_Freq_NewDriver: Current-driver version

The same 10 kHz, pot-controlled PWM with ESP-IDF 5's current MCPWM driver (`driver/mcpwm_prelude.h`), which builds with no warning and replaces the old one in future versions. It builds the chain by hand: a timer (counts 0..999 at 10 MHz), an operator, a comparator (holds the duty in ticks) and a generator (sets the pin HIGH at the start of each period and LOW at the compare value).

## Motor driver

| Part | Connect |
|---|---|
| MOSFET gate | ← 220 Ω ← GPIO14, plus 10 kΩ from gate to GND (keeps the motor off during boot) |
| MOSFET drain | → motor (−) |
| MOSFET source | → GND |
| Motor (+) | → motor supply (+) |
| Schottky diode | across the motor, stripe (cathode) on motor (+) |
| Grounds | ESP32 GND and motor supply GND joined |

The ESP32 pin only charges the MOSFET's gate; the motor current flows from its own supply through the MOSFET. The diode catches the voltage spike the motor makes each time the PWM switches off.

## What 10 kHz PWM looks like

One period is 1 ÷ 10 kHz = 0.1 ms = 100 µs. The duty is the HIGH part of each period.
