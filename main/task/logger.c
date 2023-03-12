#include "logger.h"

void logger_task(void* args) {
  static gps_t* gps_instance;
  static SemaphoreHandle_t* spi_spinlock;
  gps_instance = gps_fetch();
  spi_spinlock = fetch_spi_spinlock();

  if (xSemaphoreTake(*spi_spinlock, portMAX_DELAY) == pdTRUE) {
    storage_init(NULL);
    xSemaphoreGive(*spi_spinlock);
  }

  static char buffer[10240] = "\n";
  for (int i = 0; i < 5; i++) {
    if (xSemaphoreTake(*spi_spinlock, portMAX_DELAY) == pdTRUE) {
      storage_write((uint8_t*)buffer, sizeof(buffer));
      xSemaphoreGive(*spi_spinlock);
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
  }
  printf("closed\n");
  if (xSemaphoreTake(*spi_spinlock, portMAX_DELAY) == pdTRUE) {
    storage_deinit();
    xSemaphoreGive(*spi_spinlock);
  }
  vTaskDelete(NULL);
}
