#include "sensors.h"

#define TAG "sensors"

static uint8_t* commu_buffer;
static TimerHandle_t timer_handler;
static imu_t* imu_instance;
static pressure_sensor_t* pressure_altitude_instance;
static calibration_t cal = {
    .gyro_bias_offset = {.x = 0, .y = 0, .z = 0},
    .accel_offset = {.x = 0, .y = 0, .z = 0},
    .accel_scale_lo = {.x = 1, .y = 1, .z = 1},
    .accel_scale_hi = {.x = -1, .y = -1, .z = -1},
    .mag_offset = {.x = 0, .y = 0, .z = 0},
    .mag_scale = {.x = 1, .y = 1, .z = 1},
};
static gps_t* gps_instance;

static void sensors_loop(TimerHandle_t xTimervoid) {
  static uint32_t systick;
  systick = bsp_current_time();
  imu_update();
  bmp280_update();

  /* logging pattern ref: https://hackmd.io/s6x3UGifRqWUFJ7deyzGbw */

  uint8_t* logger_ptr = commu_buffer;
  memcpy(logger_ptr + 1, &systick, sizeof(systick));
  logger_ptr += sizeof(systick);

  memcpy(logger_ptr, &pressure_altitude_instance->relative_altitude, sizeof(pressure_altitude_instance->relative_altitude));
  logger_ptr += sizeof(pressure_altitude_instance->relative_altitude);

  memcpy(logger_ptr, &imu_instance->velocity, sizeof(imu_instance->velocity));
  logger_ptr += sizeof(imu_instance->velocity);

  memcpy(logger_ptr, &imu_instance->a, sizeof(imu_instance->a));
  logger_ptr += sizeof(imu_instance->a);

  memcpy(logger_ptr, &imu_instance->g, sizeof(imu_instance->g));
  logger_ptr += sizeof(imu_instance->g);

  memcpy(logger_ptr, &imu_instance->m, sizeof(imu_instance->m));
  logger_ptr += sizeof(imu_instance->m);

  memcpy(logger_ptr, &gps_instance->longitude, sizeof(gps_instance->longitude));
  logger_ptr += sizeof(gps_instance->longitude);

  memcpy(logger_ptr, &gps_instance->latitude, sizeof(gps_instance->latitude));
  logger_ptr += sizeof(gps_instance->latitude);

  memcpy(logger_ptr, &gps_instance->altitude, sizeof(gps_instance->altitude));
  logger_ptr += sizeof(gps_instance->altitude);

  memcpy(logger_ptr, &imu_instance->roll, sizeof(imu_instance->roll));
  logger_ptr += sizeof(imu_instance->roll);

  memcpy(logger_ptr, &imu_instance->pitch, sizeof(imu_instance->pitch));
  logger_ptr += sizeof(imu_instance->pitch);

  memcpy(logger_ptr, &imu_instance->heading, sizeof(imu_instance->heading));
  logger_ptr += sizeof(imu_instance->heading);
}

void sensors_task(void* args) {
  imu_instance = imu_fetch();
  pressure_altitude_instance = bmp_fetch();
  gps_instance = gps_fetch();
  commu_buffer = buffer_fetch();

  bmp280_init();
  gps_init();
  storage_read_config("/sd/imu", &cal, sizeof(cal));
  imu_init(&cal, sensor_freq);

  // calibration_t _cal;
  // memset(&_cal, 0, sizeof(_cal));
  // calibrate_gyro(&_cal.gyro_bias_offset);
  // calibrate_accel(&_cal.accel_offset, &_cal.accel_scale_hi, &_cal.accel_scale_lo);
  // calibrate_mag(&_cal.mag_offset, &_cal.mag_scale);
  // storage_write_config("/sd/imu", &_cal, sizeof(_cal));

  vTaskDelay(pdMS_TO_TICKS(500));

  timer_handler = xTimerCreate("sensors_loop", pdMS_TO_TICKS(1000 / sensor_freq), pdTRUE, (void*)0, sensors_loop);
  xTimerStart(timer_handler, 0);

  vTaskDelete(NULL);
}
