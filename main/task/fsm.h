#ifndef __FSM_T__
#define __FSM_T__

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdint.h>

#include "bmp280.h"
#include "gps.h"
#include "imu.h"
#include "ra01s.h"
#include "slave.h"

typedef enum {
  FSM_INIT,
  FSM_GROUND,
  FSM_RASING,
  FSM_APOGEE,
  FSM_PARACHUTE,
  FSM_DETACH,
  FSM_FALING,
  FSM_DEPLOY,
  FSM_TOUCH_GROUND,
  FSM_END,
} fsm_state_e;

void fsm_task(void*);
fsm_state_e* fsm_fetch();

#endif
