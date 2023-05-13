#include "fsm.h"

#define TAG "FSM"

void fsm_task(void*) {
  static imu_t* imu_instance;
  static fsm_state_e state = FSM_INIT;
  imu_instance = imu_fetch();
  for (;;) {
    switch (state) {
      case FSM_INIT:
        ESP_LOGI(TAG, "%d", state);
        state = FSM_GROUND;
        break;
      case FSM_GROUND:
        ESP_LOGI(TAG, "%d", state);
        if (ABS(imu_instance->a.x) > 3 || ABS(imu_instance->a.y) > 3 || ABS(imu_instance->a.z) > 3) {
          state = FSM_RASING;
        }
        break;
      case FSM_RASING:
        ESP_LOGI(TAG, "%d", state);
        break;
      case FSM_APOGEE:
        ESP_LOGI(TAG, "%d", state);
        break;
      case FSM_PARACHUTE:
        ESP_LOGI(TAG, "%d", state);
        break;
      case FSM_DEACCE:
        ESP_LOGI(TAG, "%d", state);
        break;
      case FSM_DEPLOY:
        ESP_LOGI(TAG, "%d", state);
        break;
      case FSM_TOUCH_GROUND:
        ESP_LOGI(TAG, "%d", state);
        break;
      case FSM_END:
        ESP_LOGI(TAG, "%d", state);
        break;
      default:
        break;
    }
    vTaskDelay(10);
  }
}
