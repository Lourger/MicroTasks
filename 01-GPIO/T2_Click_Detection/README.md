# T2: Single, Double Click and Long Press

One button, three gestures: print which one happened on the Serial Monitor.

## The task

Connect one push button to a GPIO pin. The program watches the button the way a computer mouse does, and prints which gesture happened: a **single click**, a **double click**, or a **long press**.

The sketch uses the timing numbers below.

## Rules

| Rule | Value |
|---|---|
| Debounce | the level must stay the same for more than `50 ms` |
| Long press | held more than `1000 ms`, reported when it is **released** |
| Double click | the 2nd press starts within `400 ms` of the **start** of the 1st press; reported as soon as the 2nd press is seen |
| Single click | reported when those `400 ms` run out with no 2nd press |
| Messages | `Single click detected`, `Double click detected`, `Long press detected` |
| Output | Serial Monitor, `115200` baud (bits per second) |

## Parts

- ESP32 DevKit
- 1 push button
- jumper wires (no resistor: the internal pull-up is used)

## Wiring

| ESP32 pin | Set up as | Connection (from the pin outward) |
|---|---|---|
| GPIO14 | INPUT_PULLUP | button → GND |

INPUT_PULLUP connects a resistor of about 45 kΩ from the pin to 3.3 V inside the chip. Released reads HIGH; pressed connects the pin to GND and reads LOW.

## Upload and run

Open `T2_Click_Detection/T2_Click_Detection.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

Serial Monitor: **115200 baud**.

## Timing: what a button really does

The contacts bounce for a few ms on press and on release. Every bounce restarts the 50 ms wait, so the stable level changes 50 ms after the last bounce.

## Timing: the three gestures

Low = pressed. Single click prints when 400 ms have passed since the press started. Double click prints the moment the 2nd press is seen. Long press prints on release. Real prints come about 50 ms later, because of the debounce wait.
