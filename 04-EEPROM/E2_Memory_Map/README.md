# E2: EEPROM Memory-Map Viewer

Print every EEPROM byte as a hex table, 16 bytes per row.

## The task

Print the whole EEPROM on the Serial Monitor: 16 bytes per row, each row starting with its address, addresses and data in hex. A header row labels the 16 columns, an ASCII column on the right shows the bytes as characters, and the table prints once at start-up.

## Rules

| Rule | Value |
|---|---|
| Size | 512 bytes (0x0000 to 0x01FF) |
| Per row | 16 bytes |
| Address | 0x%04X (4 hex digits) |
| Data | %02X (2 hex digits each) |
| ASCII | 0x20..0x7E as a character, anything else '.' |

## Parts

- ESP32 DevKit
- USB cable

## Before you start

**All EEPROM tasks share one storage area on your board.** The bytes a task saved are still there when you flash the next task, because uploading doesn't erase them. To start a task from a clean, all-`0xFF` state, erase the flash once: in Arduino IDE 2 set **Tools → Erase All Flash Before Sketch Upload → Enabled**, upload, then set it back to **Disabled** so that later uploads keep your saved data.

One more ESP32 detail: `EEPROM.begin()` with a *smaller* size than the last sketch used cuts the saved data down to the new size. A larger size keeps the old bytes and adds `0xFF` bytes at the end.

## Upload and run

Open `E2_Memory_Map/E2_Memory_Map.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

Serial Monitor: **115200 baud**.

## What the output looks like

```
EEPROM memory map (emulated EEPROM, 512 bytes)
-------+-------------------------------------------------+-----------------
Addr   | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F | ASCII
-------+-------------------------------------------------+-----------------
0x0000 | 09 05 07 FF FF FF FF FF FF FF FF FF FF FF FF FF | ................
0x0010 | FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF | ................
...
0x01F0 | FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF | ................
```

Here the first row still holds three scores from the Number Guessing Game (task E1). On a freshly erased board, every byte is `FF`. This is a good tool to check what the other EEPROM tasks really saved.
