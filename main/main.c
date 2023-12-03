#include <esp_log.h>
#include <esp_vfs.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <inttypes.h>
#include <stdio.h>

#include "bsp.h"
#include "driver/gpio.h"
#include "hal/gpio_types.h"
#include "ra01s.h"
#include "recv.h"
#include "sdkconfig.h"
#include "slave.h"
#include "task/fsm.h"
#include "task/logger.h"
#include "task/sensors.h"
#include "task/wdt.h"

static int log_vprintf(const char *fmt, va_list arguments) {
  static FILE *f = NULL;
  static int ret;
  f = storage_fetch();
  if (f != NULL)
    ret = vfprintf(f, fmt, arguments);
  else
    ret = vprintf(fmt, arguments);
  if (ret < 0) printf("Logging error: %d\n", ret);
  storage_flush();
  return ret;
}

void app_main() {
  gpio_init();
  i2c_init();
  uart_init();
  spi_init(LORA_SPI_HOST, CONFIG_LORA_MOSI_GPIO, CONFIG_LORA_MISO_GPIO, CONFIG_LORA_SCK_GPIO);
  spi_init(SD_SPI_HOST, CONFIG_SD_MOSI_GPIO, CONFIG_SD_MISO_GPIO, CONFIG_SD_SCK_GPIO);
  lora_init();
  slave_reset();

  if (sd_init() == ESP_OK) {
    printf("I am an on-board avionics board!\n");
    storage_init(NULL);
    esp_log_set_vprintf(log_vprintf);
    vTaskDelay(pdMS_TO_TICKS(1000));
    xTaskCreatePinnedToCore(fsm_task, "fsm_task", 4096, NULL, 5, NULL, 1);
    xTaskCreatePinnedToCore(sensors_task, "sensors_task", 8192, NULL, 4, NULL, 1);
    xTaskCreatePinnedToCore(logger_task, "logger_task", 4096, NULL, 3, NULL, 0);
    // xTaskCreatePinnedToCore(wdt_task, "wdt_task", 2048, NULL, 1, NULL, 1);
  } else {
    printf("I am a ground receiver board!\n");
    vTaskDelay(pdMS_TO_TICKS(1000));
    xTaskCreatePinnedToCore(recv_task, "recv_task", 8192, NULL, 5, NULL, 1);
  }

  printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());
}
