/*****************************************************************************
 *                                                                           *
 *  Copyright 2018 Simon M. Werner                                           *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *      http://www.apache.org/licenses/LICENSE-2.0                           *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 *****************************************************************************/

#ifndef __COMMON_H
#define __COMMON_H

#include <math.h>

#define SAMPLE_FREQ_Hz (CONFIG_SAMPLE_RATE_Hz)
#define SAMPLE_INTERVAL_MS (1000 / SAMPLE_FREQ_Hz)  // Sample Rate in milliseconds

typedef struct {
  float x, y, z;
} vector_t;

typedef struct {
  // Magnetometer
  vector_t mag_offset;
  vector_t mag_scale;

  // Gryoscope
  vector_t gyro_bias_offset;

  // Accelerometer
  vector_t accel_offset;
  vector_t accel_scale_lo;
  vector_t accel_scale_hi;

} calibration_t;

#define DEG2RAD(deg) (deg * M_PI / 180.0f)

void _pause(void);

#endif
