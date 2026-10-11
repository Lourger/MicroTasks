#include <Arduino.h>
#include <EEPROM.h>

// ---------- Storage ----------
#define EEPROM_SIZE     512          // bytes to show: addresses 0x0000..0x01FF
#define BYTES_PER_ROW   16

void printSeparator() {
  Serial.println("-------+-------------------------------------------------+-----------------");
}

void printRow(int rowStart) {
  Serial.printf("0x%04X | ", rowStart);                 // address: 4 upper-case hex digits

  for (int i = 0; i < BYTES_PER_ROW; i++) {            // the data bytes in hex
    int address = rowStart + i;
    if (address < EEPROM_SIZE) {
      Serial.printf("%02X ", EEPROM.read(address));
    } else {
      Serial.print("   ");                             // pad a short last row
    }
  }

  Serial.print("| ");
  for (int i = 0; i < BYTES_PER_ROW; i++) {            // the same bytes as characters
    int address = rowStart + i;
    if (address >= EEPROM_SIZE) {
      break;
    }
    uint8_t value = EEPROM.read(address);
    Serial.print((value >= 0x20 && value <= 0x7E) ? (char)value : '.');   // printable or '.'
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  delay(1000);                                         // time to open the Serial Monitor
  EEPROM.begin(EEPROM_SIZE);

  Serial.println("EEPROM memory map (emulated EEPROM, 512 bytes)");
  printSeparator();
  Serial.print("Addr   | ");
  for (int i = 0; i < BYTES_PER_ROW; i++) {
    Serial.printf("%02X ", i);                         // column offsets 00..0F
  }
  Serial.println("| ASCII");
  printSeparator();

  for (int rowStart = 0; rowStart < EEPROM_SIZE; rowStart += BYTES_PER_ROW) {
    printRow(rowStart);
  }
  printSeparator();

  EEPROM.end();                                        // done: free the RAM copy
}

void loop() {
}
