#include "sensors.h"

void sensors_task(void* args) {
  static struct bmp280_calib_param params;
  static int32_t temperature, pressure, raw_temperature, raw_pressure;

  bmp280_init();
  bmp280_get_calib_params(&params);

  gps_init();

  vTaskDelay(pdMS_TO_TICKS(500));
  for (;;) {
    bmp280_read_raw(&raw_temperature, &raw_pressure);
    temperature = bmp280_convert_temp(raw_temperature, &params);
    pressure = bmp280_convert_pressure(raw_pressure, raw_temperature, &params);

    // 1kHz
    vTaskDelay(pdMS_TO_TICKS(1));
  }
}
