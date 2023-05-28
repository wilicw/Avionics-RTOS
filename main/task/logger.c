#include "logger.h"

#define TAG "LOGGER"

static TimerHandle_t timer_handler;
static uint8_t* commu_buffer;
static uint8_t led_state = 0;

static void logger_loop(TimerHandle_t xTimervoid) {
  gpio_set_level(CONFIG_INDI_LED, led_state);
  led_state = !led_state;
  LoRaSend(commu_buffer, buffer_len(), SX126x_TXMODE_SYNC);
}

void logger_task(void* args) {
  commu_buffer = buffer_fetch();
  timer_handler = xTimerCreate("logger_loop", pdMS_TO_TICKS(10), pdTRUE, (void*)0, logger_loop);
  xTimerStart(timer_handler, 0);
  vTaskDelete(NULL);
}
