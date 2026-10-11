# U1: AT Command System via UART

Answer AT commands from the Serial Monitor: LED on/off, LED status, ADC value and voltage, uptime.

## The task

Build a small command interpreter: the ESP32 listens for AT commands on its serial port, carries each one out, and replies with a fixed response. AT commands are the text commands modems and Wi-Fi/GSM modules use; each starts with `AT`.

The sketch uses UART0 (the USB port) at 115200 baud, the LED on GPIO13 and the pot on GPIO34. It echoes each command as `Received: …`, matches commands exactly (case-sensitive), and answers `Invalid Command!` to anything else.

## Rules

| Command | Does | Response |
|---|---|---|
| AT | nothing (a link check) | OK |
| AT+LED=ON | LED on | +LED:"ON" |
| AT+LED=OFF | LED off | +LED:"OFF" |
| AT+LED_STATUS? | read the LED pin | +LED_STATUS:"ON" / "OFF" |
| AT+ADC_VALUE? | read GPIO34 | +ADC_VALUE:0..4095 |
| AT+ADC_VOLTAGE? | read GPIO34, convert | +ADC_VOLTAGE:0.00..3.30 |
| AT+SYS_ON_TIME? | time since start | +SYS_ON_TIME:"HH:MM:SS" |
| anything else |  | Invalid Command! |

## Parts

- ESP32 DevKit
- 1 LED
- 1 resistor, 150 Ω
- 1 potentiometer
- jumper wires

## Wiring

| ESP32 pin | Set up as | Connection (from the pin outward) |
|---|---|---|
| GPIO13 | output | 150 Ω resistor → LED → GND |
| GPIO34 | ADC input | 10 kΩ pot: middle leg (wiper); outer legs to 3.3 V and GND |

LEDs: the long leg (anode) faces the pin side, the short leg (cathode) faces GND.

The serial link is the USB cable: the DevKit's USB-to-UART chip connects the PC to the ESP32's UART0 (TX on GPIO1, RX on GPIO3).

## Upload and run

Open `U1_AT_Commands/U1_AT_Commands.ino` in Arduino IDE 2, select the board **ESP32 Dev Module** and upload.

Serial Monitor: **115200 baud**.

## A session in the Serial Monitor

```
AT
Received: AT
OK
AT+LED=ON
Received: AT+LED=ON
+LED:"ON"
AT+ADC_VOLTAGE?
Received: AT+ADC_VOLTAGE?
+ADC_VOLTAGE:1.65
at+led=on
Received: at+led=on
Invalid Command!
```

The lines without "Received" are what you typed; the sketch's `Received: …` echo shows what actually arrived. Lower-case commands are refused.

## Typing in the monitor

Type a command in the message box at the top of Arduino IDE 2's Serial Monitor and press Enter. The monitor sends the whole line at once, followed by the line ending chosen in the menu next to the baud rate. Choose **New Line** (or **Both NL & CR**): the sketch reads up to the newline with `readStringUntil('\n')` and removes any CR and spaces with `trim()`. With **No Line Ending** the line still arrives, but only after `readStringUntil()` gives up waiting for the newline (1 s by default).
