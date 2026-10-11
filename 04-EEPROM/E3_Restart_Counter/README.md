# E3: EEPROM-Based Restart Counter

Count every restart in EEPROM, with a serial command to reset the count.

## The task

Count how many times the ESP32 has started, after a reset or after a power cut. The count lives in EEPROM so it survives both. At each start the program asks whether to reset the count to 0.

## Rules

| Rule | Value |
|---|---|
| Counter | 1 byte at address 0 (0..255, then back to 0) |
| At start | add 1, save, print, ask Y/N |
| Y | reset to 0 and save |
| Afterwards | print the count every 5 s |

## Parts

- ESP32 DevKit
- USB cable

## Before you start

**All EEPROM tasks share one storage area on your board.** The bytes a task saved are still there when you flash the next task, because uploading doesn't erase them. To start a task from a clean, all-`0xFF` state, erase the flash once: in Arduino IDE 2 set **Tools → Erase All Flash Before Sketch Upload → Enabled**, upload, then set it back to **Disabled** so that later uploads keep your saved data.

## Upload and run

Open `E3_Restart_Counter/E3_Restart_Counter.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

Serial Monitor: **115200 baud**. When asked, type `Y` or `N` in the message box at the top and press Enter (the sketch reads the first character, so any line-ending setting works). The question appears right after start-up: if you missed it, press the board's **EN** (reset) button with the Serial Monitor open.

## Worked example: the first start

1. Erased flash: address 0 holds `0xFF = 255`
2. `255 + 1 = 256`, which doesn't fit in a `uint8_t`: it wraps to `0`
3. So the first start prints `Restart count: 0`, the next `1`, and so on
4. After `255` the count wraps to `0` again

If an earlier task left a byte at address 0, the count starts from that value instead. Answer `Y` once to start from 0 (or erase the flash, see *Before you start*).
