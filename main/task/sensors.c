#include "sensors.h"

#include <stddef.h>
#include <stdint.h>

#include "bmp280.h"

#define TAG "sensors"

static uint8_t uuid;
static uint8_t* commu_buffer;
static TimerHandle_t timer_handler;
static TimerHandle_t imu_timer_handler;
static imu_t* imu_instance;
static fsm_state_e* state;
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
  // imu_update();
  bmp280_update();

  /* logging pattern ref: https://hackmd.io/s6x3UGifRqWUFJ7deyzGbw */

  uint8_t* logger_ptr = commu_buffer;
  memcpy(logger_ptr, &uuid, sizeof(uuid));
  logger_ptr += sizeof(uuid);

  memcpy(logger_ptr, &state, sizeof(fsm_state_e));
  logger_ptr += sizeof(fsm_state_e);

  memcpy(logger_ptr, &systick, sizeof(systick));
  logger_ptr += sizeof(systick);

  memcpy(logger_ptr, &pressure_altitude_instance->relative_altitude, sizeof(pressure_altitude_instance->relative_altitude));
  logger_ptr += sizeof(pressure_altitude_instance->relative_altitude);

  memcpy(logger_ptr, &pressure_altitude_instance->velocity, sizeof(pressure_altitude_instance->velocity));
  logger_ptr += sizeof(pressure_altitude_instance->velocity);

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

  uint8_t ecc = 0;
  for (size_t i = 0; i < logger_ptr - commu_buffer; i++) {
    ecc = ecc ^ commu_buffer[i];
  }
  memcpy(logger_ptr, &ecc, sizeof(ecc));
  logger_ptr += sizeof(ecc);

  ESP_LOGI(TAG, "%u,%u,%lu,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%ld,%ld,%f,%f,%f,%f\n",
           uuid, *state, systick, pressure_altitude_instance->relative_altitude,
           pressure_altitude_instance->velocity,
           imu_instance->a.x, imu_instance->a.y, imu_instance->a.z,
           imu_instance->g.x, imu_instance->g.y, imu_instance->g.z,
           imu_instance->m.x, imu_instance->m.y, imu_instance->m.z,
           gps_instance->longitude, gps_instance->latitude, gps_instance->altitude, imu_instance->roll, imu_instance->pitch, imu_instance->heading);
}

void sensors_task(void* args) {
  imu_instance = imu_fetch();
  pressure_altitude_instance = bmp_fetch();
  gps_instance = gps_fetch();
  commu_buffer = buffer_fetch();
  state = fsm_fetch();
  storage_read_config("/sd/uuid", &uuid, 1);

  bmp280_init();
  gps_init();
  // storage_read_config("/sd/imu", &cal, sizeof(cal));
  // imu_init(&cal, 100);

  // calibration_t _cal;
  // memset(&_cal, 0, sizeof(_cal));
  // calibrate_gyro(&_cal.gyro_bias_offset);
  // calibrate_accel(&_cal.accel_offset, &_cal.accel_scale_hi, &_cal.accel_scale_lo);
  // calibrate_mag(&_cal.mag_offset, &_cal.mag_scale);
  // storage_write_config("/sd/imu", &_cal, sizeof(_cal));
  // memcpy(&cal, &_cal, sizeof(calibration_t));

  vTaskDelay(pdMS_TO_TICKS(500));

  timer_handler = xTimerCreate("sensors_loop", pdMS_TO_TICKS(1000 / sensor_freq), pdTRUE, (void*)0, sensors_loop);
  xTimerStart(timer_handler, 0);

  vTaskDelete(NULL);
}
