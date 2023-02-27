#include "wdt.h"

void wdt_task(void* args) {
  static gps_t* gps_instance;
  gps_instance = gps_fetch();

  slave_reset();

  for (size_t i = 0; i < 4; i++) {
    buzzer(50);
    vTaskDelay(pdMS_TO_TICKS(100));
  }

  vTaskDelay(pdMS_TO_TICKS(1000));

  for (;;) {
    if (gps_instance->ready == 0) {
      rgb(128, 0, 0);
      buzzer(50);
    } else {
      rgb(128, 128, 0);
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
