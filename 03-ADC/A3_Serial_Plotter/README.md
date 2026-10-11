# A3: ADC Serial Plotter

Turn the pot and watch its ADC reading as a live graph.

## The task

Read a potentiometer with the ADC and send the readings to a serial plotter, so you see the value as a moving graph. Each line is one raw reading (0..4095), 10 per second.

## Rules

| Rule | Value |
|---|---|
| Input | GPIO34, 12-bit |
| Output | one number per line, no text |
| Rate | every 100 ms |
| Baud | 115200 |

## Parts

- ESP32 DevKit
- 1 potentiometer, 1 kΩ to 10 kΩ
- jumper wires

## Wiring

| ESP32 pin | Set up as | Connection (from the pin outward) |
|---|---|---|
| GPIO34 | ADC input | 10 kΩ pot: middle leg (wiper); outer legs to 3.3 V and GND |

The sketch reads GPIO34, an ADC1 pin. ADC1 pins keep working while Wi-Fi is on; ADC2 pins don't.

## Upload and run

This task has more than one version of the program. Each runs on its own on the same board and wiring; flash the one you want.

| Sketch | Board / version | Serial Monitor |
|---|---|---|
| [`A3_Serial_Plotter`](A3_Serial_Plotter/A3_Serial_Plotter.ino) | Arduino IDE Serial Plotter version | 115200 baud |
| [`A3_Serial_Plotter_Teleplot`](A3_Serial_Plotter_Teleplot/A3_Serial_Plotter_Teleplot.ino) | Teleplot (VS Code) version | 115200 baud |

For every sketch: select the board **ESP32 Dev Module** in Arduino IDE 2, open the `.ino` file and upload.

### A3_Serial_Plotter_Teleplot: Teleplot version

For the Teleplot extension in VS Code: the same program, with `>pot:` in front of each number so Teleplot knows which series the value belongs to.

## Seeing the graph

- **Arduino IDE 2:** flash `A3_Serial_Plotter`, open **Tools → Serial Plotter** and set it to 115200 baud. It draws the plain numbers the sketch prints.
- **VS Code:** the **Teleplot** extension needs each line as `>name:value`; flash `A3_Serial_Plotter_Teleplot` for that.
