#include "sensors.h"

#define TAG "sensors"

static TimerHandle_t timer100hz;
static imu_t* imu_instance;
static struct bmp280_calib_param params;
static int32_t temperature, pressure, raw_temperature, raw_pressure;

static void sensors_loop(TimerHandle_t xTimervoid) {
  bmp280_read_raw(&raw_temperature, &raw_pressure);
  temperature = bmp280_convert_temp(raw_temperature, &params);
  pressure = bmp280_convert_pressure(raw_pressure, raw_temperature, &params);
  imu_update();
  ESP_LOGI(TAG, "%f %f %f", imu_instance->heading, imu_instance->pitch, imu_instance->roll);
}

void sensors_task(void* args) {
  imu_instance = imu_fetch();

  bmp280_init();
  bmp280_get_calib_params(&params);
  gps_init();
  imu_init(100);

  vTaskDelay(pdMS_TO_TICKS(500));

  timer100hz = xTimerCreate("sensors_loop", pdMS_TO_TICKS(10), pdTRUE, (void*)0, sensors_loop);
  xTimerStart(timer100hz, 0);

  vTaskDelete(NULL);
}
