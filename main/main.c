#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <inttypes.h>
#include <stdio.h>

#include "bsp.h"
#include "slave.h"
#include "task/commu.h"
#include "task/sensors.h"
#include "wdt.h"

void app_main() {
  gpio_init();
  i2c_init();
  uart_init();
  spi_init();

  xTaskCreatePinnedToCore(sensors_task, "sensors_task", 4096, NULL, 7, NULL, 0);
  xTaskCreatePinnedToCore(commu_task, "commu_task", 4096, NULL, 8, NULL, 0);
  xTaskCreatePinnedToCore(wdt_task, "wdt_task", 2048, NULL, 5, NULL, 1);

  printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());
}
