#include "fsm.h"

#include <stdint.h>

#include "bmp280.h"
#include "bsp.h"
#include "driver/gpio.h"

#define TAG "FSM"
#define FSM_DELAY 50

static fsm_state_e state = FSM_INIT;
static fsm_state_e last_state = 0;
const float apogee_vel_range = 0.5;

void fsm_task(void* args) {
  vTaskDelay(3000);
  static imu_t* imu_instance;
  static pressure_sensor_t* pressure_instance;
  imu_instance = imu_fetch();
  pressure_instance = bmp_fetch();
  static uint32_t launch_time = 0, flight_time;
  for (;;) {
    // printf("%d\n", gpio_get_level(CONFIG_INT_IO));
    if (state >= 2) {
      flight_time = bsp_current_time() - launch_time;
    } else {
      flight_time = 0;
    }
    if (last_state != state) {
      printf("%d\n", state);
      last_state = state;
    }
    switch (state) {
      case FSM_INIT:
        state = FSM_GROUND;
        break;
      case FSM_GROUND:
        if (pressure_instance->relative_altitude >= 20) {
          state = FSM_RASING;
          launch_time = bsp_current_time();
        }
        break;
      case FSM_RASING:
        if (flight_time >= 3000 && flight_time < 30000) {
          if (pressure_instance->relative_altitude <= 20) {
            state = FSM_GROUND;
          }
        } else if (flight_time >= 30000) {
          state = FSM_APOGEE;
        }
        break;
      case FSM_APOGEE:
        state = FSM_PARACHUTE;
        break;
      case FSM_PARACHUTE: {
        /* Deploy parachute */
        gpio_set_level(CONFIG_FIRE_1, 0);
        state = FSM_DETACH;
        break;
      }
      case FSM_DETACH:
        if (gpio_get_level(CONFIG_INT_IO) == 1) {
          vTaskDelay(100);
          if (gpio_get_level(CONFIG_INT_IO) == 1) {
            vTaskDelay(3000);
            state = FSM_FALING;
            servo(0, 70);
            vTaskDelay(1000);
          }
        }
        break;
      case FSM_FALING:
        if (pressure_instance->relative_altitude <= 125) {
          state = FSM_DEPLOY;
        }
        break;
      case FSM_DEPLOY:
        gpio_set_level(CONFIG_FIRE_2, 0);
        state = FSM_TOUCH_GROUND;
        break;
      case FSM_TOUCH_GROUND:
        if (flight_time >= 600 * 1000) {
          state = FSM_END;
        }
        break;
      case FSM_END:
        esp_log_set_vprintf(vprintf);
        vTaskDelete(NULL);
        break;
      default:
        break;
    }
    vTaskDelay(1);
  }
}

fsm_state_e* fsm_fetch() {
  return &state;
}
