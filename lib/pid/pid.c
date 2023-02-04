#include "pid.h"

#define PID_FUNC_INIT(PID_TYPE)                                \
  void pid_##PID_TYPE##_init(pid_##PID_TYPE##_config_t *pid) { \
    pid->i = 0;                                                \
    pid->last_value = 0;                                       \
    pid->output = 0;                                           \
  }

#define PID_FUNC_UPDATE(PID_TYPE)                                                                   \
  void pid_##PID_TYPE##_update(pid_##PID_TYPE##_config_t *pid, PID_TYPE value) {                    \
    PID_TYPE error = pid->setpoint - value;                                                         \
                                                                                                    \
    pid->i += pid->gain.ki * error;                                                                 \
    pid->i = CLAMP(pid->i, pid->limit.max, pid->limit.min);                                         \
                                                                                                    \
    pid->output = pid->gain.kp * error + pid->i + -1.0f * pid->gain.kd * (value - pid->last_value); \
    pid->output = CLAMP(pid->output, pid->limit.max, pid->limit.min);                               \
    pid->last_value = value;                                                                        \
  }

PID_FUNC_INIT(float)
PID_FUNC_UPDATE(float)
