#include "bsp.h"

#define TAG "BSP"

static QueueHandle_t uart_queue;
static spi_device_handle_t spi_handle;
static uint8_t ignitor[] = {GPIO_FIRE_1, GPIO_FIRE_2};
static SemaphoreHandle_t spi_spinlock;

void gpio_init() {
  static const gpio_config_t io_conf = {
      .intr_type = GPIO_INTR_DISABLE,
      .mode = GPIO_MODE_OUTPUT,
      .pin_bit_mask = GPIO_OUTPUT_PIN_SEL,
      .pull_down_en = 0,
      .pull_up_en = 0,
  };
  gpio_config(&io_conf);
  for (ssize_t i = 0; i < sizeof(ignitor); i++)
    gpio_set_level(ignitor[i], 1);
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

void spi_init() {
  static esp_err_t ret;
  static spi_device_interface_config_t devcfg;
  static const spi_bus_config_t spi_bus_config = {
      .sclk_io_num = CONFIG_SCLK_GPIO,
      .mosi_io_num = CONFIG_MOSI_GPIO,
      .miso_io_num = CONFIG_MISO_GPIO,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
  };

  ret = spi_bus_initialize(SPI_HOST, &spi_bus_config, SPI_DMA_CH_AUTO);
  ESP_LOGI(TAG, "spi_bus_initialize=%d", ret);

  memset(&devcfg, 0, sizeof(spi_device_interface_config_t));
  devcfg.clock_speed_hz = SPI_FREQ_HZ;
  // It does work with software CS control.
  devcfg.spics_io_num = -1;
  devcfg.queue_size = 5;
  devcfg.mode = 0;
  devcfg.flags = SPI_DEVICE_NO_DUMMY;

  // spi_device_handle_t handle;
  ret = spi_bus_add_device(SPI_HOST, &devcfg, &spi_handle);
  ESP_LOGI(TAG, "spi_bus_add_device=%d", ret);
}

void sd_init() {
  sdmmc_card_t* card;
  const char mount_point[] = SD_MOUNT;
  sdmmc_host_t host = SDSPI_HOST_DEFAULT();
  sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
      .format_if_mount_failed = true,
      .max_files = 5,
      .allocation_unit_size = 16 * 1024,
  };
  slot_config.gpio_cs = CONFIG_SD_NSS_GPIO;
  slot_config.host_id = host.slot;
  esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);
  sdmmc_card_print_info(stdout, card);
}

spi_device_handle_t* fetch_spi_handler() {
  return &spi_handle;
}

QueueHandle_t* fetch_uart_queue() {
  return &uart_queue;
}

SemaphoreHandle_t* fetch_spi_spinlock() {
  static uint8_t sem_created = 0;
  if (!sem_created) spi_spinlock = xSemaphoreCreateMutex();
  sem_created = 1;
  return &spi_spinlock;
}
