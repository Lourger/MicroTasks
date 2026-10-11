# E4: Measurement Unit Configuration

A serial menu for unit preferences, saved in EEPROM as one struct.

## The task

Let the user choose a unit for four quantities: temperature (Celsius or Fahrenheit), distance (kilometres or miles), weight (kilograms or pounds) and volume (litres or gallons). The choices are saved in EEPROM, so they survive a restart, and a menu option restores the factory defaults. Only the settings part is needed; no real measurements.

The four choices are one `struct` of four bytes, saved with `EEPROM.put()` and loaded with `EEPROM.get()`.

## Rules

| Option | Does |
|---|---|
| 1 to 4 | choose the unit for temperature, distance, weight or volume (then type 0 or 1) |
| 5 | restore factory defaults (all 0: Celsius, km, kg, litres) |
| at start | load and show the saved preferences; invalid data → defaults |

## Parts

- ESP32 DevKit
- USB cable

## Before you start

**All EEPROM tasks share one storage area on your board.** The bytes a task saved are still there when you flash the next task, because uploading doesn't erase them. To start a task from a clean, all-`0xFF` state, erase the flash once: in Arduino IDE 2 set **Tools → Erase All Flash Before Sketch Upload → Enabled**, upload, then set it back to **Disabled** so that later uploads keep your saved data.

## Upload and run

Open `E4_Unit_Config/E4_Unit_Config.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

Serial Monitor: **115200 baud**. To type in the Serial Monitor, use the message box at the top and press Enter. Set the line-ending menu next to the baud rate to **New Line** (or **Both NL & CR**).

## How the struct sits in EEPROM

| Address | Field | Example | Erased flash |
|---|---|---|---|
| 0 | temperature | 1 | 0xFF |
| 1 | distance | 0 | 0xFF |
| 2 | weight | 0 | 0xFF |
| 3 | volume | 1 | 0xFF |

The example column is Fahrenheit, km, kg and gallons (0 = the first unit of each pair, 1 = the second). `sizeof(UnitPreferences)` is 4, so `put()` copies exactly these 4 bytes. `0xFF` is not a valid choice (only 0 or 1 are), which is how the program spots an erased EEPROM, or bytes left over from another task.
