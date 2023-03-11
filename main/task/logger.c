#include "logger.h"

void logger_task(void* args) {
  static gps_t* gps_instance;
  gps_instance = gps_fetch();
  storage_init(NULL);
  static char buffer[10240] = "\n";
  static UBaseType_t op;
  for (int i = 0; i < 5; i++) {
    op = uxTaskPriorityGet(NULL);
    vTaskPrioritySet(NULL, configMAX_PRIORITIES - 1);
    storage_write((uint8_t*)buffer, sizeof(buffer));
    vTaskPrioritySet(NULL, op);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
  printf("closed\n");
  storage_deinit();
  vTaskDelete(NULL);
}
