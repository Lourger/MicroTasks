# E6: Password Protected Locker System

A PIN locker on the Serial Monitor: user PIN in EEPROM, master PIN to reset it.

## The task

Build a locker controlled from the Serial Monitor, with two PINs:

- **User PIN:** opens the locker. It is set the first time the locker starts, and can be changed after typing the current one.
- **Master PIN:** fixed in the program; it resets the user PIN.

The user PIN is 4 digits stored as 4 ASCII bytes at addresses 0 to 3, and an erased address 0 (`0xFF`) means "first use".

## Rules

| Option | Does |
|---|---|
| first start | ask for a new 4-digit PIN and save it |
| 1 | unlock with the user PIN |
| 2 | change the PIN (current PIN first) |
| 3 | reset with the master PIN 1234, then set and print the new PIN |

## Parts

- ESP32 DevKit
- USB cable

## Before you start

**All EEPROM tasks share one storage area on your board.** The bytes a task saved are still there when you flash the next task, because uploading doesn't erase them. To start a task from a clean, all-`0xFF` state, erase the flash once: in Arduino IDE 2 set **Tools → Erase All Flash Before Sketch Upload → Enabled**, upload, then set it back to **Disabled** so that later uploads keep your saved data.

Here it matters most: if an earlier task left something other than `0xFF` at address 0, the locker thinks a PIN is already set. Use option 3 with the master PIN to set your own.

## Upload and run

Open `E6_Locker/E6_Locker.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

Serial Monitor: **115200 baud**. To type in the Serial Monitor, use the message box at the top and press Enter. Set the line-ending menu next to the baud rate to **New Line** (or **Both NL & CR**).

## What is stored

| Address | 0 | 1 | 2 | 3 |
|---|---|---|---|---|
| PIN "4821" (ASCII) | 0x34 | 0x38 | 0x32 | 0x31 |
| first use | 0xFF | 0xFF | 0xFF | 0xFF |

The digit `'0'` is byte `0x30`, `'1'` is `0x31`, and so on. **Demo code:** the PIN sits in flash as plain text, and anyone with the board can read it (the Memory-Map Viewer, task E2, shows it). A real lock would store a hash of the PIN and limit wrong attempts.
