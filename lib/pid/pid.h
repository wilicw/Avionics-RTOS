#ifndef __PID_L__
#define __PID_L__

/* clamping macros */
#define CLAMP(x, max, min) ((x) > (max) ? (max) : ((x) < (min) ? (min) : (x)))

#define DECLARE_PID(PID_TYPE)                              \
  typedef struct pid {                                     \
    PID_TYPE setpoint;                                     \
    struct {                                               \
      PID_TYPE kp;                                         \
      PID_TYPE ki;                                         \
      PID_TYPE kd;                                         \
    } gain;                                                \
    struct {                                               \
      PID_TYPE max;                                        \
      PID_TYPE min;                                        \
    } limit;                                               \
    PID_TYPE last_value;                                   \
    PID_TYPE i;                                            \
    PID_TYPE output;                                       \
  } pid_##PID_TYPE##_config_t;                             \
                                                           \
  void pid_##PID_TYPE##_init(pid_##PID_TYPE##_config_t *); \
  void pid_##PID_TYPE##_update(pid_##PID_TYPE##_config_t *, PID_TYPE);

DECLARE_PID(float)

#endif
