# I2: LED Toggling Using I2C

A button on one ESP32 toggles an LED on another ESP32 over I2C.

## The task

Connect two microcontrollers over I2C. The **master** reads a push button; the **slave** drives an LED. Each press toggles the LED.

The master sends one byte (1 = on, 0 = off) to the slave at address 0x08, the slave sets its LED from that byte, and external 4.7 kΩ pull-ups replace the internal ones.

## Rules

| Board | Pins | Does |
|---|---|---|
| Master | button GPIO15; SDA 21, SCL 22 | on each press: flip the state, send 1 byte, print OK or the error code |
| Slave | LED GPIO2 (150 Ω); SDA 21, SCL 22 | address 0x08; 1 → LED on, 0 → LED off |
| Bus | 100 kHz, 4.7 kΩ pull-ups | internal pull-ups switched off |

## Parts

- 2 ESP32 DevKits
- 1 push button
- 1 LED
- 1 resistor, 150 Ω
- 2 resistors, 4.7 kΩ
- jumper wires

## Wiring

| Bus line | Master | Slave ESP32 |
|---|---|---|
| SDA | GPIO21 | GPIO21 |
| SCL | GPIO22 | GPIO22 |
| GND | GND | GND |
| I2C address | - | address 0x08 |

Pull-ups: one 4.7 kΩ resistor from SDA to 3.3 V and one from SCL to 3.3 V, once for the whole bus.

SDA to SDA, SCL to SCL (not crossed like UART's TX and RX), and the two GNDs connected.

## Upload and run

This task uses two ESP32 boards. Flash each board with its own sketch:

| Sketch | Board / version | Serial Monitor |
|---|---|---|
| [`I2_LED_Toggle_Master`](I2_LED_Toggle_Master/I2_LED_Toggle_Master.ino) | Master ESP32 | 115200 baud |
| [`I2_LED_Toggle_Slave`](I2_LED_Toggle_Slave/I2_LED_Toggle_Slave.ino) | Slave ESP32 | 115200 baud |

For every sketch: select the board **ESP32 Dev Module** in Arduino IDE 2, open the `.ino` file and upload.

## Button on the master, LED on the slave

| Pin (on its own board) | Set up as | Connection (from the pin outward) |
|---|---|---|
| GPIO15 | INPUT_PULLUP | master button → GND |
| GPIO2 | output | 150 Ω resistor → slave LED → GND |

LEDs: the long leg (anode) faces the pin side, the short leg (cathode) faces GND.

## One press on the wires

1. START, then the address byte `0x08 << 1 | 0 = 0x10` (address + write bit), slave answers ACK
2. Data byte `0x01`, slave answers ACK
3. STOP. The slave's Wire library now calls `onReceive(1)`
4. 18 clock pulses (2 bytes × 9) at 100 kHz ≈ `0.2 ms` on the wires
