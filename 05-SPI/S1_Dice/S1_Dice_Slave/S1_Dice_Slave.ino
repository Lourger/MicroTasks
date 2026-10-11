#include <Arduino.h>
#include <driver/spi_slave.h>
#include <driver/gpio.h>

// ---------- SPI slave pins (VSPI) ----------
#define SLAVE_SCK_PIN   18
#define SLAVE_MISO_PIN  19
#define SLAVE_MOSI_PIN  23
#define SLAVE_CS_PIN    5

// ---------- One-byte buffers (word aligned, as the driver prefers) ----------
WORD_ALIGNED_ATTR uint8_t txByte[4];
WORD_ALIGNED_ATTR uint8_t rxByte[4];

void setup() {
  Serial.begin(115200);

  spi_bus_config_t bus = {};
  bus.mosi_io_num   = SLAVE_MOSI_PIN;
  bus.miso_io_num   = SLAVE_MISO_PIN;
  bus.sclk_io_num   = SLAVE_SCK_PIN;
  bus.quadwp_io_num = -1;
  bus.quadhd_io_num = -1;

  spi_slave_interface_config_t slave = {};
  slave.spics_io_num = SLAVE_CS_PIN;
  slave.mode         = 2;             // must match the master's SPI_MODE2
  slave.queue_size   = 1;

  // keep CS HIGH (not selected) when no master is connected
  gpio_set_pull_mode((gpio_num_t)SLAVE_CS_PIN, GPIO_PULLUP_ONLY);

  esp_err_t err = spi_slave_initialize(VSPI_HOST, &bus, &slave, SPI_DMA_DISABLED);
  Serial.println(err == ESP_OK ? "Dice slave ready." : "SPI slave init failed!");
}

void loop() {
  txByte[0] = (uint8_t)random(1, 7);  // 1..6, ready before the master asks

  spi_slave_transaction_t t = {};
  t.length    = 8;                    // bits
  t.tx_buffer = txByte;
  t.rx_buffer = rxByte;

  // waits here until the master selects us and clocks one byte
  if (spi_slave_transmit(VSPI_HOST, &t, portMAX_DELAY) == ESP_OK) {
    Serial.print("Sent roll: ");
    Serial.println(txByte[0]);
  }
}
