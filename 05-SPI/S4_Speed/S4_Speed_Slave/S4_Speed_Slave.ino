#include <Arduino.h>
#include <Wire.h>
#include <driver/spi_slave.h>
#include <driver/gpio.h>

// ---------- I2C slave (SDA 21, SCL 22) ----------
#define I2C_ADDRESS     9

// ---------- SPI slave (VSPI) ----------
#define SLAVE_SCK_PIN   18
#define SLAVE_MISO_PIN  19
#define SLAVE_MOSI_PIN  23
#define SLAVE_CS_PIN    5
#define BLOCK_SIZE      1000        // one CS-low block from the master
#define QUEUED_BLOCKS   3           // buffers waiting for the master

// ---------- Report ----------
#define REPORT_BYTES      100000    // print once this many bytes arrived
#define PRINT_INTERVAL_MS 1000

WORD_ALIGNED_ATTR uint8_t rxBlocks[QUEUED_BLOCKS][BLOCK_SIZE];
spi_slave_transaction_t transactions[QUEUED_BLOCKS];

volatile uint32_t i2cCount = 0;     // written by the I2C receive callback
uint32_t spiCount = 0;
uint32_t lastPrintedI2C = 0;
uint32_t lastPrintedSPI = 0;
unsigned long lastPrintTime = 0;

void onI2CReceive(int howMany) {    // runs after each I2C write from the master
  uint32_t received = 0;
  while (Wire.available()) {
    Wire.read();
    received++;
  }
  i2cCount = i2cCount + received;
}

void printIfNew(const char *label, uint32_t count, uint32_t &lastPrinted) {
  if (count >= REPORT_BYTES && count != lastPrinted) {
    Serial.print(label);
    Serial.print(count / 1000.0, 3);   // 1 KB = 1000 bytes here
    Serial.println(" KB");
    lastPrinted = count;
  }
}

void setup() {
  Serial.begin(115200);

  Wire.onReceive(onI2CReceive);
  Wire.begin((uint8_t)I2C_ADDRESS);

  spi_bus_config_t bus = {};
  bus.mosi_io_num     = SLAVE_MOSI_PIN;
  bus.miso_io_num     = SLAVE_MISO_PIN;
  bus.sclk_io_num     = SLAVE_SCK_PIN;
  bus.quadwp_io_num   = -1;
  bus.quadhd_io_num   = -1;
  bus.max_transfer_sz = BLOCK_SIZE;

  spi_slave_interface_config_t slave = {};
  slave.spics_io_num = SLAVE_CS_PIN;
  slave.mode         = 0;
  slave.queue_size   = QUEUED_BLOCKS;

  gpio_set_pull_mode((gpio_num_t)SLAVE_CS_PIN, GPIO_PULLUP_ONLY);
  spi_slave_initialize(VSPI_HOST, &bus, &slave, SPI_DMA_CH_AUTO);  // DMA: blocks over 64 bytes

  for (int i = 0; i < QUEUED_BLOCKS; i++) {
    transactions[i] = {};
    transactions[i].length    = BLOCK_SIZE * 8;   // bits
    transactions[i].rx_buffer = rxBlocks[i];      // receive only, nothing to send back
    spi_slave_queue_trans(VSPI_HOST, &transactions[i], portMAX_DELAY);
  }
  Serial.println("Slave ready: I2C address 9 and SPI.");
}

void loop() {
  // collect finished SPI blocks without waiting, then queue each buffer again
  spi_slave_transaction_t *done;
  while (spi_slave_get_trans_result(VSPI_HOST, &done, 0) == ESP_OK) {
    spiCount += done->trans_len / 8;   // bits actually received -> bytes
    spi_slave_queue_trans(VSPI_HOST, done, portMAX_DELAY);
  }

  if (millis() - lastPrintTime >= PRINT_INTERVAL_MS) {
    lastPrintTime = millis();
    printIfNew("I2C data received: ", i2cCount, lastPrintedI2C);
    printIfNew("SPI data received: ", spiCount, lastPrintedSPI);
  }
}
