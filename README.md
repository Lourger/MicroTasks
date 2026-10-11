# MicroTasks

50 ESP32 microcontroller tasks with full solutions as standalone Arduino IDE sketches: GPIO, PWM, ADC, EEPROM, SPI, I2C, interrupts, UART, sleep, timers and DAC.

[![Compile sketches](https://github.com/Lourger/MicroTasks/actions/workflows/compile-sketches.yml/badge.svg)](https://github.com/Lourger/MicroTasks/actions/workflows/compile-sketches.yml)

Every task folder has a README with the task, its rules, the parts, the wiring and notes on how the solution works. Every sketch folder holds exactly one `.ino` file and nothing else, so you can copy any single sketch folder on its own and open it in Arduino IDE 2.

## Setup

1. Install [Arduino IDE 2](https://www.arduino.cc/en/software).
2. Open **File → Preferences** and add this line to **Additional boards manager URLs**:  
   `https://espressif.github.io/arduino-esp32/package_esp32_index.json`
3. Open **Tools → Board → Boards Manager**, search for **esp32**, pick **esp32 by Espressif Systems**, choose version **3.3.12** and click **Install**.
4. Select **Tools → Board → esp32 → ESP32 Dev Module**, connect the board and select its port under **Tools → Port**.
5. Open a sketch with **File → Open** (for example `03-ADC/A1_Simple_Voltmeter/A1_Simple_Voltmeter/A1_Simple_Voltmeter.ino`) and click **Upload**.
6. Open **Tools → Serial Monitor** at the baud rate given in the task's README (115200 for almost every sketch).

No library needs to be installed. Every library the sketches use (`Wire`, `SPI`, `EEPROM`, `SD` and the ESP32 driver headers such as `driver/ledc.h`, `driver/pulse_cnt.h`, `driver/spi_slave.h` and `esp_sleep.h`) comes with the ESP32 core.

Tasks marked with 2 boards use two ESP32 DevKits, each with its own sketch; the task README says which board gets which sketch.

## Tasks

| Section | Task | Boards | Folder |
|---|---|---|---|
| GPIO | T1: LED Blink (2 versions) | 1 | [01-GPIO/T1_LED_Blink](01-GPIO/T1_LED_Blink/) |
| GPIO | T2: Single, Double Click and Long Press | 1 | [01-GPIO/T2_Click_Detection](01-GPIO/T2_Click_Detection/) |
| GPIO | T3: Logic Gate Implementation | 1 | [01-GPIO/T3_Logic_Gates](01-GPIO/T3_Logic_Gates/) |
| GPIO | T4: LED Blinking Patterns | 1 | [01-GPIO/T4_Blink_Patterns](01-GPIO/T4_Blink_Patterns/) |
| GPIO | T5: 4-bit Binary Counter | 1 | [01-GPIO/T5_Binary_Counter](01-GPIO/T5_Binary_Counter/) |
| PWM | P1: LED Brightness Control Using PWM | 1 | [02-PWM/P1_LED_PWM_Ramp](02-PWM/P1_LED_PWM_Ramp/) |
| PWM | P2: LED Brightness Control Using Potentiometer | 1 | [02-PWM/P2_Pot_Brightness](02-PWM/P2_Pot_Brightness/) |
| PWM | P3: Bar Graph | 1 | [02-PWM/P3_Bar_Graph](02-PWM/P3_Bar_Graph/) |
| PWM | P4: Seesaw Effect Using LEDs (2 versions) | 1 | [02-PWM/P4_Seesaw](02-PWM/P4_Seesaw/) |
| PWM | P5: LEDs Fading Speed Control | 1 | [02-PWM/P5_Fade_Speed](02-PWM/P5_Fade_Speed/) |
| PWM | P6: PWM High-Frequency Generation (2 versions) | 1 | [02-PWM/P6_PWM_High_Freq](02-PWM/P6_PWM_High_Freq/) |
| ADC | A1: Simple Voltmeter Using ADC | 1 | [03-ADC/A1_Simple_Voltmeter](03-ADC/A1_Simple_Voltmeter/) |
| ADC | A2: Precision Voltmeter Using ADC | 1 | [03-ADC/A2_Precision_Voltmeter](03-ADC/A2_Precision_Voltmeter/) |
| ADC | A3: ADC Serial Plotter (2 versions) | 1 | [03-ADC/A3_Serial_Plotter](03-ADC/A3_Serial_Plotter/) |
| ADC | A4: Interface LM35 Temperature Sensor | 1 | [03-ADC/A4_LM35](03-ADC/A4_LM35/) |
| ADC | A5: Sine Wave Analyzer | 1 | [03-ADC/A5_Sine_Analyzer](03-ADC/A5_Sine_Analyzer/) |
| ADC | A6: Smart OhmMeter | 1 | [03-ADC/A6_Ohmmeter](03-ADC/A6_Ohmmeter/) |
| EEPROM | E1: Number Guessing Game | 1 | [04-EEPROM/E1_Guessing_Game](04-EEPROM/E1_Guessing_Game/) |
| EEPROM | E2: EEPROM Memory-Map Viewer | 1 | [04-EEPROM/E2_Memory_Map](04-EEPROM/E2_Memory_Map/) |
| EEPROM | E3: EEPROM-Based Restart Counter | 1 | [04-EEPROM/E3_Restart_Counter](04-EEPROM/E3_Restart_Counter/) |
| EEPROM | E4: Measurement Unit Configuration | 1 | [04-EEPROM/E4_Unit_Config](04-EEPROM/E4_Unit_Config/) |
| EEPROM | E5: People Counter with EEPROM | 1 | [04-EEPROM/E5_People_Counter](04-EEPROM/E5_People_Counter/) |
| EEPROM | E6: Password Protected Locker System | 1 | [04-EEPROM/E6_Locker](04-EEPROM/E6_Locker/) |
| SPI | S1: SPI Dice Roll | 2 | [05-SPI/S1_Dice](05-SPI/S1_Dice/) |
| SPI | S2: LED Toggle Using SPI | 2 | [05-SPI/S2_LED](05-SPI/S2_LED/) |
| SPI | S3: Multi-Master SPI Communication | 2 | [05-SPI/S3_Multimaster](05-SPI/S3_Multimaster/) |
| SPI | S4: SPI vs I2C Speed Comparison | 2 | [05-SPI/S4_Speed](05-SPI/S4_Speed/) |
| SPI | S5: BMP280 Data Logger | 1 | [05-SPI/S5_BMP280_Logger](05-SPI/S5_BMP280_Logger/) |
| I2C | I1: I2C Scanner | 1 | [06-I2C/I1_Scanner](06-I2C/I1_Scanner/) |
| I2C | I2: LED Toggling Using I2C | 2 | [06-I2C/I2_LED_Toggle](06-I2C/I2_LED_Toggle/) |
| I2C | I3: LED Brightness Control Using I2C | 2 | [06-I2C/I3_Brightness](06-I2C/I3_Brightness/) |
| I2C | I4: User-Defined RTC Functions for Time Reading | 1 | [06-I2C/I4_RTC_Read](06-I2C/I4_RTC_Read/) |
| I2C | I5: Multi-Master Interface with I2C 16x2 LCD | 2 | [06-I2C/I5_LCD](06-I2C/I5_LCD/) |
| I2C | I6: Multi-Slave I2C Communication | 1 | [06-I2C/I6_Multi_Slave](06-I2C/I6_Multi_Slave/) |
| Interrupts | N1: LED Patterns Switching | 1 | [07-Interrupt/N1_Pattern_Switch](07-Interrupt/N1_Pattern_Switch/) |
| Interrupts | N2: Short Pulse Detection | 2 | [07-Interrupt/N2_Pulse_Detect](07-Interrupt/N2_Pulse_Detect/) |
| Interrupts | N3: Single, Double and Long Press with Interrupts | 1 | [07-Interrupt/N3_Click_Detect](07-Interrupt/N3_Click_Detect/) |
| UART | U1: AT Command System via UART | 1 | [08-UART/U1_AT_Commands](08-UART/U1_AT_Commands/) |
| UART | U2: UART-Based LEDs Control | 2 | [08-UART/U2_LEDs_Control](08-UART/U2_LEDs_Control/) |
| UART | U3: PathFinder Compass | 1 | [08-UART/U3_Pathfinder](08-UART/U3_Pathfinder/) |
| Sleep | Z1: Sleep Mode with Button Press Wake-Up | 1 | [09-Sleep/Z1_Button_Wake](09-Sleep/Z1_Button_Wake/) |
| Sleep | Z2: Sleep Mode with Periodic Wake-Up | 1 | [09-Sleep/Z2_Timer_Wake](09-Sleep/Z2_Timer_Wake/) |
| Sleep | Z3: I2C Slave with Sleep Mode | 2 | [09-Sleep/Z3_I2C_Sleep](09-Sleep/Z3_I2C_Sleep/) |
| Timers | T1: Simultaneous Blinking LEDs | 1 | [10-Timer/T1_Dual_Blink](10-Timer/T1_Dual_Blink/) |
| Timers | T2: Simultaneous Square Wave Generation (2 versions) | 1 | [10-Timer/T2_Square_Waves](10-Timer/T2_Square_Waves/) |
| Timers | T3: High-Frequency Pulses Counter | 2 | [10-Timer/T3_Pulse_Counter](10-Timer/T3_Pulse_Counter/) |
| Timers | T4: High-Frequency Measurement | 2 | [10-Timer/T4_Freq_Meter](10-Timer/T4_Freq_Meter/) |
| DAC | D1: Triangular Wave Generator | 1 | [11-DAC/D1_Triangle](11-DAC/D1_Triangle/) |
| DAC | D2: Multi-Waveform Generator | 1 | [11-DAC/D2_Multi_Wave](11-DAC/D2_Multi_Wave/) |
| DAC | D3: Phase-shifted Sine Wave | 1 | [11-DAC/D3_Phase_Sine](11-DAC/D3_Phase_Sine/) |

## Layout

```
<NN>-<Section>/<Task>/README.md                  the task, wiring and notes
<NN>-<Section>/<Task>/<Sketch>/<Sketch>.ino      one complete sketch
```

A task with two boards, or with two versions of its program, has one sketch folder per program inside its task folder.

## Compile check

On every push to `main` (and when started by hand from the **Actions** tab), the [compile-sketches](.github/workflows/compile-sketches.yml) workflow compiles every sketch folder for **ESP32 Dev Module** (`esp32:esp32:esp32`) with esp32 by Espressif Systems **3.3.12**, using the `arduino/compile-sketches` GitHub Action from Arduino.
