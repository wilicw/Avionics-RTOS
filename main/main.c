#include <esp_log.h>
#include <esp_vfs.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <inttypes.h>
#include <stdio.h>

#include "bsp.h"
#include "slave.h"
#include "task/commu.h"
#include "task/logger.h"
#include "task/sensors.h"
#include "task/wdt.h"

static FILE *f = NULL;

static int log_vprintf(const char *fmt, va_list arguments) {
  if (f == NULL) printf(fmt, arguments);
  return vfprintf(f, fmt, arguments);
}

void app_main() {
  gpio_init();
  i2c_init();
  uart_init();
  spi_init();
  sd_init();
  storage_init(NULL);

  f = storage_fetch();
  esp_log_set_vprintf(log_vprintf);

  vTaskDelay(pdMS_TO_TICKS(1000));

  xTaskCreatePinnedToCore(sensors_task, "sensors_task", 4096, NULL, 4, NULL, 0);
  xTaskCreatePinnedToCore(logger_task, "logger_task", 4096, NULL, 3, NULL, 0);
  xTaskCreatePinnedToCore(commu_task, "commu_task", 4096, NULL, 2, NULL, 0);
  // xTaskCreatePinnedToCore(wdt_task, "wdt_task", 2048, NULL, 1, NULL, 1);

  printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());
}
