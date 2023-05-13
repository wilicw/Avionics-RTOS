#ifndef __FSM_T__
#define __FSM_T__

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "gps.h"
#include "imu.h"
#include "ra01s.h"

#define ABS(x) ((x) > (0) ? (x) : -(x))

typedef enum {
  FSM_INIT,
  FSM_GROUND,
  FSM_RASING,
  FSM_APOGEE,
  FSM_PARACHUTE,
  FSM_DEACCE,
  FSM_DEPLOY,
  FSM_TOUCH_GROUND,
  FSM_END,
} fsm_state_e;

void fsm_task(void*);

#endif
