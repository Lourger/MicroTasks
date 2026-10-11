# U2: UART-Based LEDs Control

Two ESP32s over UART: a pot on one dims an LED on the other; a button on the other toggles an LED on the first.

## The task

Two microcontrollers talk over UART in both directions. Microcontroller 1 has a potentiometer and LED1; Microcontroller 2 has a push button and LED2. The pot on 1 sets the brightness of LED2 on 2, and the button on 2 toggles LED1 on 1. Microcontroller 2 prints the ADC values it receives; Microcontroller 1 prints LED1's state.

Both boards use `Serial2` on GPIO16/17 at 115200 baud, single-byte commands (`'T'` to toggle, 0 to 4 to fetch the reading byte by byte), a 100 ms poll, and 12-bit PWM for LED2.

## Rules

| Board | Has | Does |
|---|---|---|
| Microcontroller 1 | pot GPIO34, LED1 GPIO4 | answers commands; prints LED is ON / OFF |
| Microcontroller 2 | button GPIO15, LED2 GPIO4 (PWM) | polls every 100 ms; sends 'T' on a press; prints Received ADC Value: N |
| Link | Serial2, RX 16, TX 17, 115200 8N1 | TX of one board to RX of the other |

## Parts

- 2 ESP32 DevKits
- 1 potentiometer
- 2 LEDs
- 2 resistors, 150 Ω
- 1 push button
- jumper wires

## Wiring

| Signal | MCU 1 | MCU 2 | Direction |
|---|---|---|---|
| TX → RX | GPIO17 TX | GPIO16 RX | MCU 1 → MCU 2 |
| RX ← TX | GPIO16 RX | GPIO17 TX | MCU 2 → MCU 1 |
| GND | GND | GND | common ground |

UART lines cross: each board's TX goes to the other's RX. Unlike I2C, there are no pull-ups and no addresses: it's one sender per wire.

## Upload and run

This task uses two ESP32 boards. Flash each board with its own sketch:

| Sketch | Board / version | Serial Monitor |
|---|---|---|
| [`U2_MCU1_Pot_LED1`](U2_MCU1_Pot_LED1/U2_MCU1_Pot_LED1.ino) | Microcontroller 1 (pot + LED1) | 115200 baud |
| [`U2_MCU2_Button_LED2`](U2_MCU2_Button_LED2/U2_MCU2_Button_LED2.ino) | Microcontroller 2 (button + LED2) | 115200 baud |

For every sketch: select the board **ESP32 Dev Module** in Arduino IDE 2, open the `.ino` file and upload.

## Parts on each board

| MCU 1 / MCU 2 pin | Set up as | Connection (from the pin outward) |
|---|---|---|
| GPIO34 | ADC input | 10 kΩ pot: middle leg (wiper); outer legs to 3.3 V and GND |
| GPIO4 | output | 150 Ω resistor → LED1 → GND |
| GPIO15 | INPUT_PULLUP | button → GND |
| GPIO4 | output | 150 Ω resistor → LED2 → GND |

LEDs: the long leg (anode) faces the pin side, the short leg (cathode) faces GND.

Top two rows on MCU 1 (pot on GPIO34, LED1 on GPIO4); bottom two on MCU 2 (button on GPIO15, LED2 on GPIO4).

## One poll, byte by byte

| MCU 2 sends | MCU 1 does | MCU 1 replies |
|---|---|---|
| 0 | reads the pot (say 2048) | 0 (acknowledge) |
| 1 |  | 0xFF (4095 low) |
| 2 |  | 0x0F (4095 high) |
| 3 |  | 0x00 (2048 low) |
| 4 |  | 0x08 (2048 high) |

Each byte at 115200 baud takes 10 bit times: about 87 µs. The whole poll, with the 1 ms gaps, takes about 5 ms out of every 100 ms.

## How a byte travels

'A' = 0x41 = 0100 0001. The line rests HIGH. A start bit (LOW), then the 8 data bits with the lowest bit first (1, 0, 0, 0, 0, 0, 1, 0), then a stop bit (HIGH). 10 bit times per byte.
