# E1: Number Guessing Game

Guess a number from 1 to 100 and keep a leaderboard that survives a reset.

## The task

The ESP32 picks a random number from 1 to 100. You type guesses in the Serial Monitor and it answers `Too High!` or `Too Low!` until you get it. The number of attempts is your score.

Scores are saved in EEPROM, so the leaderboard is still there after a reset or a power cut. The last 10 scores are kept, the leaderboard shows them sorted from fewest attempts, and your new score is marked `(You)`.

## Rules

| Rule | Value |
|---|---|
| Secret number | random 1..100 |
| Answers | Too High! / Too Low! / Correct! |
| Storage | last 10 scores, 1 byte each, address 0..9 (newest at 0) |
| Empty slot | 0 or 255 (0xFF) |
| After a win | save, show the leaderboard, wait 2 s, new game |

## Parts

- ESP32 DevKit
- USB cable (for the Serial Monitor)

## Before you start

**All EEPROM tasks share one storage area on your board.** The bytes a task saved are still there when you flash the next task, because uploading doesn't erase them. To start a task from a clean, all-`0xFF` state, erase the flash once: in Arduino IDE 2 set **Tools → Erase All Flash Before Sketch Upload → Enabled**, upload, then set it back to **Disabled** so that later uploads keep your saved data.

## Upload and run

Open `E1_Guessing_Game/E1_Guessing_Game.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

Serial Monitor: **115200 baud**. To type in the Serial Monitor, use the message box at the top and press Enter. Set the line-ending menu next to the baud rate to **New Line** (or **Both NL & CR**).

## What the 10 bytes look like

After three games with scores 7, then 5, then 9 (9 is the newest):

| Address | 0 | 1 | 2 | 3 .. 9 |
|---|---|---|---|---|
| Value | 9 | 5 | 7 | 255 (0xFF, empty) |

A new score of 4 moves every byte one address up (the score at address 9 drops out) and writes 4 at address 0. The leaderboard then sorts the scores it found: `4 (You), 5, 7, 9`.
