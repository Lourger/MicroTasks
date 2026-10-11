# E5: People Counter with EEPROM

Two buttons count people in and out; the count survives power loss.

## The task

Build a practice people counter: one button adds a person, the other removes one. The count prints on the Serial Monitor and is saved in EEPROM after every change, so a reset or power cut doesn't lose it.

The count is a 4-byte `uint32_t` saved with `put()`, and it never goes below 0.

## Rules

| Rule | Value |
|---|---|
| INC button | GPIO15, INPUT_PULLUP |
| DEC button | GPIO16, INPUT_PULLUP |
| Debounce | 50 ms |
| Storage | uint32_t at address 0, saved after every change |
| Lower limit | 0 (DEC at 0 is ignored) |

## Parts

- ESP32 DevKit
- 2 push buttons
- jumper wires

## Wiring

| ESP32 pin | Set up as | Connection (from the pin outward) |
|---|---|---|
| GPIO15 | INPUT_PULLUP | INC button → GND |
| GPIO16 | INPUT_PULLUP | DEC button → GND |

Each button connects its pin to GND. The internal pull-ups keep the pins HIGH until pressed.

## Before you start

**All EEPROM tasks share one storage area on your board.** The bytes a task saved are still there when you flash the next task, because uploading doesn't erase them. To start a task from a clean, all-`0xFF` state, erase the flash once: in Arduino IDE 2 set **Tools → Erase All Flash Before Sketch Upload → Enabled**, upload, then set it back to **Disabled** so that later uploads keep your saved data.

## Upload and run

Open `E5_People_Counter/E5_People_Counter.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

Serial Monitor: **115200 baud**.

## How a uint32_t is stored

1. Count 300 in hex: `0x0000012C`
2. The ESP32 is little-endian: the lowest byte goes first
3. Address 0..3 hold `2C 01 00 00`
4. Erased flash reads `FF FF FF FF` = `0xFFFFFFFF`, which the program treats as "never saved"

Bytes left over from an earlier task are read as a count too. For example, a locker PIN "1234" from task E6 (`31 32 33 34`) would start the counter at 875,770,417. Erase the flash once before this task (see *Before you start*).
