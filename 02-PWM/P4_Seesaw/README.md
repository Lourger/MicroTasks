# P4: Seesaw Effect Using LEDs

Five LEDs tilt like a seesaw: one end brightens while the other dims.

## The task

Five LEDs, LED0 to LED4, and a potentiometer. With the pot at 0 %, LED0 is off, LED4 is fully on, and the LEDs in between step evenly from 0 % to 100 %. Turning the pot towards 100 % tilts the seesaw: LED0 brightens and LED4 dims.

This is the "linear" seesaw (`P4_Seesaw`). A second, "dynamic" version (`P4_Seesaw_Dynamic`) is described under *Upload and run*.

## Rules

| LED | GPIO | Pot 0 % | Pot 50 % | Pot 100 % |
|---|---|---|---|---|
| LED0 | 15 | 0 % | 50 % | 100 % |
| LED1 | 16 | 25 % | 50 % | 75 % |
| LED2 | 17 | 50 % | 50 % | 50 % |
| LED3 | 18 | 75 % | 50 % | 25 % |
| LED4 | 19 | 100 % | 50 % | 0 % |

## Parts

- ESP32 DevKit
- 5 LEDs
- 5 resistors, 150 Ω
- 1 potentiometer, 1 kΩ to 10 kΩ
- jumper wires

## Wiring

| ESP32 pin | Set up as | Connection (from the pin outward) |
|---|---|---|
| GPIO34 | ADC input | 10 kΩ pot: middle leg (wiper); outer legs to 3.3 V and GND |
| GPIO15 | output | 150 Ω resistor → LED0 → GND |
| GPIO16 | output | 150 Ω resistor → LED1 → GND |
| GPIO17 | output | 150 Ω resistor → LED2 → GND |
| GPIO18 | output | 150 Ω resistor → LED3 → GND |
| GPIO19 | output | 150 Ω resistor → LED4 → GND |

LEDs: the long leg (anode) faces the pin side, the short leg (cathode) faces GND.

## Upload and run

This task has more than one version of the program. Each runs on its own on the same board and wiring; flash the one you want.

| Sketch | Board / version | Serial Monitor |
|---|---|---|
| [`P4_Seesaw`](P4_Seesaw/P4_Seesaw.ino) | Linear version | not used |
| [`P4_Seesaw_Dynamic`](P4_Seesaw_Dynamic/P4_Seesaw_Dynamic.ino) | Dynamic version | not needed (opens the port at 115200 baud, prints nothing) |

For every sketch: select the board **ESP32 Dev Module** in Arduino IDE 2, open the `.ino` file and upload.

### P4_Seesaw_Dynamic: Dynamic version

In this version the end LEDs still go 0 → 100 % and 100 → 0 %, but LED1 and LED3 swing between 4 % and 90 %, and the middle LED rises to 80 % at the centre and falls back to 12 % at both ends. It updates every 10 ms.

## Each LED's duty as the pot turns

At pot 50 % all five LEDs have the same 50 % duty: that's the seesaw's pivot, where every LED is at half brightness.
