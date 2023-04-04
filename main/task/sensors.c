#include "sensors.h"

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
static void transform_accel_gyro(vector_t *v) {
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
static void transform_mag(vector_t *v) {
  float x = v->x;
  float y = v->y;
  float z = v->z;

  v->x = -y;
  v->y = z;
  v->z = -x;
}

void sensors_task(void *args) {
  static struct bmp280_calib_param params;
  static int32_t temperature, pressure, raw_temperature, raw_pressure;

  bmp280_init();
  bmp280_get_calib_params(&params);

  gps_init();

  i2c_mpu9250_init(&cal);

  MadgwickAHRSinit(100, 0.8);

  vTaskDelay(pdMS_TO_TICKS(500));

  // calibrate_gyro();
  // calibrate_accel();
  // calibrate_mag();

  static vector_t va, vg, vm;

  for (;;) {
    bmp280_read_raw(&raw_temperature, &raw_pressure);
    temperature = bmp280_convert_temp(raw_temperature, &params);
    pressure = bmp280_convert_pressure(raw_pressure, raw_temperature, &params);

    // Get the Accelerometer, Gyroscope and Magnetometer values.
    ESP_ERROR_CHECK(get_accel_gyro_mag(&va, &vg, &vm));
    // printf("%f %f %f %f %f %f\n", va.x, va.y, va.z, vg.x, vg.y, vg.z);
    // Transform these values to the orientation of our device.
    transform_accel_gyro(&va);
    transform_accel_gyro(&vg);
    transform_mag(&vm);

    // Apply the AHRS algorithm
    MadgwickAHRSupdate(
        DEG2RAD(vg.x), DEG2RAD(vg.y), DEG2RAD(vg.z),
        va.x, va.y, va.z,
        vm.x, vm.y, vm.z);
    float heading, pitch, roll;
    MadgwickGetEulerAnglesDegrees(&heading, &pitch, &roll);
    printf("heading: %2.3f°, pitch: %2.3f°, roll: %2.3f°\n", heading, pitch, roll);
    // 1kHz
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}
