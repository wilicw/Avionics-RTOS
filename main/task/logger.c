#include "logger.h"

#define TAG "LOGGER"

static uint8_t* commu_buffer;
static uint8_t led_state = 0;

void logger_task(void* args) {
  commu_buffer = buffer_fetch();
  while (1) {
    gpio_set_level(CONFIG_INDI_LED, led_state);
    led_state = !led_state;
    LoRaSend(commu_buffer, buffer_len(), SX126x_TXMODE_SYNC);
    vTaskDelay(pdMS_TO_TICKS(1));
  }
}
