#ifndef __IMU_H__
#define __IMU_H__

#include <math.h>

#include "MadgwickAHRS.h"
#include "calibrate.h"
#include "common.h"
#include "esp_system.h"
#include "mpu9250.h"

typedef struct {
  uint32_t freq;
  vector_t a, g, m;
  vector_t velocity;
  float heading, pitch, roll;
} imu_t;

void imu_init(calibration_t*, uint32_t);
void imu_update();
imu_t* imu_fetch();

#endif
