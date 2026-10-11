#include <Arduino.h>
#include <driver/spi_slave.h>

// ---------- SPI slave pins (VSPI) ----------
#define SLAVE_MOSI_PIN  23
#define SLAVE_MISO_PIN  19
#define SLAVE_SCLK_PIN  18
#define SLAVE_CS_PIN    5

// ---------- LED ----------
#define LED_PIN         15          // LED + 150 ohm resistor to GND

// ---------- Protocol ----------
#define CMD_LED_ON      0x01
#define REPLY_BYTE      0xAA        // what the master receives back

WORD_ALIGNED_ATTR uint8_t rxBuffer[4];
WORD_ALIGNED_ATTR uint8_t txBuffer[4];

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);      // LED on at startup

  spi_bus_config_t busConfig = {};
  busConfig.mosi_io_num   = SLAVE_MOSI_PIN;
  busConfig.miso_io_num   = SLAVE_MISO_PIN;
  busConfig.sclk_io_num   = SLAVE_SCLK_PIN;
  busConfig.quadwp_io_num = -1;
  busConfig.quadhd_io_num = -1;

  spi_slave_interface_config_t slaveConfig = {};
  slaveConfig.spics_io_num = SLAVE_CS_PIN;
  slaveConfig.mode         = 0;      // same mode as the master
  slaveConfig.queue_size   = 1;

  spi_slave_initialize(VSPI_HOST, &busConfig, &slaveConfig, SPI_DMA_DISABLED);
  Serial.println("SPI slave ready.");
}

void loop() {
  txBuffer[0] = REPLY_BYTE;

  spi_slave_transaction_t trans = {};
  trans.length    = 8;              // one byte, in bits
  trans.tx_buffer = txBuffer;
  trans.rx_buffer = rxBuffer;

  spi_slave_transmit(VSPI_HOST, &trans, portMAX_DELAY);  // wait for the master

  uint8_t command = rxBuffer[0];
  digitalWrite(LED_PIN, command == CMD_LED_ON ? HIGH : LOW);
  Serial.printf("Received: 0x%02X\n", command);
}
