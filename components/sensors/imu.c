#include "imu.h"

// const static vector_t rot = {
//     .x = 90.0 * (M_PI) / 180.0,
//     .y = 0.0 * (M_PI) / 180.0,
//     .z = 90.0 * (M_PI) / 180.0,
// };

calibration_t *cal = NULL;
static imu_t imu_instance;
static float rotation[3][3] = {
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1},
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

static inline void rotate(vector_t *v) {
  const float x = v->x * rotation[0][0] + v->y * rotation[0][1] + v->z * rotation[0][2];
  const float y = v->x * rotation[1][0] + v->y * rotation[1][1] + v->z * rotation[1][2];
  const float z = v->x * rotation[2][0] + v->y * rotation[2][1] + v->z * rotation[2][2];
  v->x = x;
  v->y = y;
  v->z = z;
}

void imu_init(calibration_t *_cal, uint32_t frequency) {
  /* Init the MPU and AHRS */
  cal = _cal;
  i2c_mpu9250_init(cal);
  MadgwickAHRSinit(frequency, 0.8);
  imu_instance.freq = frequency;
  imu_instance.velocity.x = 0;
  imu_instance.velocity.y = 0;
  imu_instance.velocity.z = 0;

  /* Construct the rotation matrix */
  // rotation[0][0] = cos(rot.y) * cos(rot.z) - sin(rot.x) * sin(rot.y) * sin(rot.z);
  // rotation[0][1] = -1.0 * cos(rot.x) * sin(rot.z);
  // rotation[0][2] = sin(rot.y) * cos(rot.z) + sin(rot.x) * cos(rot.y) * sin(rot.z);
  // rotation[1][0] = cos(rot.y) * sin(rot.z) + sin(rot.x) * sin(rot.y) * cos(rot.z);
  // rotation[1][1] = cos(rot.x) * cos(rot.z);
  // rotation[1][2] = sin(rot.y) * sin(rot.z) - sin(rot.x) * cos(rot.y) * cos(rot.z);
  // rotation[2][0] = -1.0 * cos(rot.x) * sin(rot.y);
  // rotation[2][1] = sin(rot.x);
  // rotation[2][2] = cos(rot.x) * cos(rot.y);
}

void imu_update() {
  // Get the Accelerometer, Gyroscope and Magnetometer imu_instance.alues.
  ESP_ERROR_CHECK(get_accel_gyro_mag(&imu_instance.a, &imu_instance.g, &imu_instance.m));
  // Transform these imu_instance.alues to the orientation of our device.
  transform_accel_gyro(&imu_instance.a);
  transform_accel_gyro(&imu_instance.g);
  transform_mag(&imu_instance.m);

  rotate(&imu_instance.a);
  rotate(&imu_instance.g);
  rotate(&imu_instance.m);

  // Apply the AHRS algorithm
  MadgwickAHRSupdate(
      DEG2RAD(imu_instance.g.x), DEG2RAD(imu_instance.g.y), DEG2RAD(imu_instance.g.z),
      imu_instance.a.x, imu_instance.a.y, imu_instance.a.z,
      imu_instance.m.x, imu_instance.m.y, imu_instance.m.z);

  MadgwickGetEulerAnglesDegrees(&imu_instance.heading, &imu_instance.pitch, &imu_instance.roll);
  imu_instance.velocity.x += imu_instance.a.x / imu_instance.freq;
  imu_instance.velocity.y += imu_instance.a.y / imu_instance.freq;
  imu_instance.velocity.z += imu_instance.a.z / imu_instance.freq;
}

imu_t *imu_fetch() {
  return &imu_instance;
}
