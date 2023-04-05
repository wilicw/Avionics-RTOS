#include "logger.h"

void logger_task(void* args) {
  static SemaphoreHandle_t* spi_spinlock;
  spi_spinlock = fetch_spi_spinlock();

  for (;;) {
    if (xSemaphoreTake(*spi_spinlock, portMAX_DELAY) == pdTRUE) {
      storage_flush();
      xSemaphoreGive(*spi_spinlock);
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
  }
}
