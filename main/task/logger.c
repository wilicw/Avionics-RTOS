#include "logger.h"

void logger_task(void* args) {
  uint8_t data[128] = {0};
  for (;;) {
    LoRaSend(data, sizeof(data), SX126x_TXMODE_SYNC);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
