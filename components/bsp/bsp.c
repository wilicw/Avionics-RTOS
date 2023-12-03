#include "bsp.h"

#include "driver/gpio.h"

#define TAG "BSP"

static QueueHandle_t uart_queue;
static uint8_t ignitor[] = {GPIO_FIRE_1, GPIO_FIRE_2};

void gpio_init() {
  for (ssize_t i = 0; i < sizeof(ignitor); i++) {
    gpio_set_level(ignitor[i], 1);
    const gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << ignitor[i]),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    gpio_set_level(ignitor[i], 1);
    gpio_config(&io_conf);
  }

  gpio_set_level(CONFIG_INDI_LED, 0);
  gpio_set_direction(CONFIG_INDI_LED, GPIO_MODE_OUTPUT);

  gpio_set_level(CONFIG_INT_IO, 1);
  gpio_set_direction(CONFIG_INT_IO, GPIO_MODE_INPUT);
  gpio_pullup_en(CONFIG_INT_IO);
}

void i2c_init() {
  static const i2c_config_t conf = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = I2C_MASTER_SDA_IO,
      .scl_io_num = I2C_MASTER_SCL_IO,
      .sda_pullup_en = GPIO_PULLUP_DISABLE,
      .scl_pullup_en = GPIO_PULLUP_DISABLE,
      .master.clk_speed = I2C_MASTER_FREQ_HZ,
  };
  i2c_param_config(I2C_MASTER_NUM, &conf);
  i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

void uart_init() {
  const static uart_config_t uart_config = {
      .baud_rate = 9600,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .source_clk = UART_SCLK_DEFAULT,
  };
  if (uart_driver_install(GPS_UART_NUM, 256, 0, 20, &uart_queue, 0) != ESP_OK)
    printf("UART install failed\n");
  if (uart_param_config(GPS_UART_NUM, &uart_config) != ESP_OK)
    printf("UART param failed\n");
  if (uart_set_pin(GPS_UART_NUM, 18, 17, -1, -1) != ESP_OK)
    printf("UART set pin failed\n");
  if (uart_enable_pattern_det_baud_intr(GPS_UART_NUM, '\n', 1, 9, 0, 0) != ESP_OK)
    printf("UART enable interrupt failed\n");
  if (uart_pattern_queue_reset(GPS_UART_NUM, 20) != ESP_OK)
    printf("UART pattern failed\n");
  uart_flush(GPS_UART_NUM);
  printf("GPS init...\n");
}

void spi_init(spi_host_device_t device, uint32_t mosi, uint32_t miso, uint32_t sck) {
  esp_err_t ret;
  const spi_bus_config_t spi_bus_config = {
      .sclk_io_num = sck,
      .mosi_io_num = mosi,
      .miso_io_num = miso,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
  };
  ret = spi_bus_initialize(device, &spi_bus_config, SPI_DMA_CH_AUTO);
  ESP_LOGI(TAG, "SPI%d_HOST spi_bus_initialize=%d", device + 1, ret);
}

esp_err_t sd_init() {
  esp_err_t err;
  sdmmc_card_t* card;
  const char mount_point[] = SD_MOUNT;
  sdmmc_host_t host = SDSPI_HOST_DEFAULT();
  sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
      .format_if_mount_failed = true,
      .max_files = 5,
      .allocation_unit_size = 16 * 1024,
  };
  host.slot = SD_SPI_HOST;
  slot_config.gpio_cs = CONFIG_SD_NSS_GPIO;
  slot_config.host_id = host.slot;
  err = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);
  if (err != ESP_OK) return err;
  sdmmc_card_print_info(stdout, card);
  return ESP_OK;
}

uint32_t bsp_current_time() {
  return xTaskGetTickCount();
}

// x    : input
// y    : last output
// fc   : cut-off frequency
// fs   : sampling frequency
float lpf(float x, float y, float fc, float fs) {
  float tau = 1 / (2.0 * M_PI * fc);
  float alpha = (1.0 / fs) / (tau + (1.0 / fs));
  return y + alpha * (x - y);
}

float iir_1st(float x, float y, float a) {
  return (1 - a) * x + a * y;
}

QueueHandle_t* fetch_uart_queue() {
  return &uart_queue;
}
