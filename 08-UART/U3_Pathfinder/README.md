# U3: PathFinder Compass

Read a GPS position over UART and print the distance (km) and compass direction to a fixed camp.

## The task

A soldier's GPS device sends their position over UART as text: `X=latitude,Y=longitude`. Work out how far the camp is and in which direction, and print both. Without a GPS module, type the position into the Serial Monitor.

The sketch uses 9600 baud on the USB serial port, the camp at 18.556283, 73.955067, the haversine formula for distance (Earth radius 6371 km), the initial bearing for direction, and 8 compass points of 45° each.

## Rules

| Rule | Value |
|---|---|
| Input | X=18.543070,Y=73.852389 (a space after the comma is fine) |
| Camp | X=18.556283, Y=73.955067 |
| Baud | 9600: set the Serial Monitor to 9600 for this task |
| Output | Distance to camp: 10.92 km / Direction to camp: E |
| Bad input | Error: Invalid GPS format |

## Parts

- ESP32 DevKit
- (optional) a GPS module that can send this text format

## Upload and run

Open `U3_Pathfinder/U3_Pathfinder.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

Serial Monitor: **9600 baud**.

## The test positions

```
X=18.543070,Y=73.852389
X=18.447045,Y=73.858581
X=17.692949,Y=74.003719
X=18.145256,Y=74.575520
X=18.500245,Y=73.858700
```

| Test | Expected | This sketch prints |
|---|---|---|
| 1 | 10.92 km, E | 10.92 km, E |
| 2 | 15.85 km, NE | **15.84** km, NE |
| 3 | 96.14 km, N | 96.14 km, N |
| 4 | 79.85 km, NW | **79.86** km, NW |
| 5 | 11.92 km, NE | 11.92 km, NE |

Two results differ in the last digit. With full `double` precision, the exact distances are 15.8448 km and 79.8551 km, which round to 15.84 and 79.86. The expected values were most likely worked out with 32-bit `float` math, which shifts those two values just across the rounding point.

## Worked example: direction

1. Position 18.543070, 73.852389 → bearing to camp `≈ 82.3°` (0° = north, 90° = east)
2. Sector: `(82.3 + 22.5) ÷ 45 = 2.33` → whole part `2`
3. `DIRECTIONS[2] = "E"`: east covers 67.5° to 112.5°
4. The 22.5° shift centres each sector: N covers 337.5° to 22.5°

## Monitor speed

This task uses 9600 baud, not 115200. Set the Serial Monitor's baud menu to **9600** while you work on it, and back to 115200 for the other tasks. If the speeds differ, you see garbage characters: the receiver samples the bits at the wrong moments.

Type a position such as `X=18.543070,Y=73.852389` in the message box at the top of Arduino IDE 2's Serial Monitor and press Enter. The monitor sends the whole line at once, followed by the line ending chosen in the menu next to the baud rate. Choose **New Line** (or **Both NL & CR**): the sketch reads up to the newline with `readStringUntil('\n')` and removes any CR and spaces with `trim()`. With **No Line Ending** the line still arrives, but only after `readStringUntil()` gives up waiting for the newline (1 s by default).
