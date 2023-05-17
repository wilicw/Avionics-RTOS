#include "sensors.h"

#define TAG "sensors"

static TimerHandle_t timer100hz;
static imu_t* imu_instance;
static pressure_sensor_t* pressure_altitude_instance;
static calibration_t cal = {
    .gyro_bias_offset = {.x = -5.158530, .y = -5.315794, .z = 0.360488},
    .accel_offset = {.x = 0.041205, .y = 0.160501, .z = 0.175093},
    .accel_scale_lo = {.x = 1.000718, .y = 1.053886, .z = 1.076510},
    .accel_scale_hi = {.x = -0.958496, .y = -0.906022, .z = -0.910636},
    .mag_offset = {.x = -24.423828, .y = 98.613281, .z = 20.097656},
    .mag_scale = {.x = 0.878383, .y = 1.105444, .z = 1.045008},
};

static void sensors_loop(TimerHandle_t xTimervoid) {
  imu_update();
  bmp280_update();
  ESP_LOGI(TAG, "%f %f %f %f %f %f",
           imu_instance->heading, imu_instance->pitch, imu_instance->roll,
           imu_instance->a.x, imu_instance->a.y, imu_instance->a.z);
}

void sensors_task(void* args) {
  imu_instance = imu_fetch();
  pressure_altitude_instance = bmp_fetch();

  bmp280_init();
  gps_init();
  imu_init(&cal, 100);

  vTaskDelay(pdMS_TO_TICKS(500));

  timer100hz = xTimerCreate("sensors_loop", pdMS_TO_TICKS(10), pdTRUE, (void*)0, sensors_loop);
  xTimerStart(timer100hz, 0);

  vTaskDelete(NULL);
}
