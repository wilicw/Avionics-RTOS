#include "imu.h"

static imu_t imu_instance;

static calibration_t cal = {
    // .mag_offset = {.x = 0.0, .y = 0.0, .z = 0.0},
    // .mag_scale = {.x = 1.0, .y = 1.0, .z = 1.0},
    // .accel_offset = {.x = 0.0, .y = 0.0, .z = 0.0},
    // .accel_scale_lo = {.x = -1.0, .y = -1.0, .z = -1.0},
    // .accel_scale_hi = {.x = 1.0, .y = 1.0, .z = 1.0},
    // .gyro_bias_offset = {.x = 0.0, .y = 0.0, .z = 0.0}

    .accel_offset = {.x = 0.008023, .y = -0.013835, .z = 0.047973},
    .accel_scale_lo = {.x = 1.002415, .y = 0.991621, .z = 1.022268},
    .accel_scale_hi = {.x = -0.993995, .y = -1.005425, .z = -0.980493},
    .gyro_bias_offset = {.x = 0.576727, .y = 0.532752, .z = 1.436956},
    .mag_offset = {.x = 1.179688, .y = 20.121094, .z = 0.568359},
    .mag_scale = {.x = 0.824076, .y = 1.214923, .z = 1.037967},
};

/**
 * Transformation:
 *  - Rotate around Z axis 180 degrees
 *  - Rotate around X axis -90 degrees
 * @param  {object} s {x,y,z} sensor
 * @return {object}   {x,y,z} transformed
 */
static inline void transform_accel_gyro(vector_t *v) {
  float x = v->x;
  float y = v->y;
  float z = v->z;

  v->x = -x;
  v->y = -z;
  v->z = -y;
}

/**
 * Transformation: to get magnetometer aligned
 * @param  {object} s {x,y,z} sensor
 * @return {object}   {x,y,z} transformed
 */
static inline void transform_mag(vector_t *v) {
  float x = v->x;
  float y = v->y;
  float z = v->z;

  v->x = -y;
  v->y = z;
  v->z = -x;
}

void imu_init(uint32_t frequency) {
  i2c_mpu9250_init(&cal);
  MadgwickAHRSinit(frequency, 0.8);
  // calibrate_gyro();
  // calibrate_accel();
  // calibrate_mag();
}

void imu_update() {
  // Get the Accelerometer, Gyroscope and Magnetometer imu_instance.alues.
  ESP_ERROR_CHECK(get_accel_gyro_mag(&imu_instance.a, &imu_instance.g, &imu_instance.m));
  // Transform these imu_instance.alues to the orientation of our device.
  transform_accel_gyro(&imu_instance.a);
  transform_accel_gyro(&imu_instance.g);
  transform_mag(&imu_instance.m);
  // Apply the AHRS algorithm
  MadgwickAHRSupdate(
      DEG2RAD(imu_instance.g.x), DEG2RAD(imu_instance.g.y), DEG2RAD(imu_instance.g.z),
      imu_instance.a.x, imu_instance.a.y, imu_instance.a.z,
      imu_instance.m.x, imu_instance.m.y, imu_instance.m.z);

  MadgwickGetEulerAnglesDegrees(&imu_instance.heading, &imu_instance.pitch, &imu_instance.roll);
}

imu_t *imu_fetch() {
  return &imu_instance;
}
