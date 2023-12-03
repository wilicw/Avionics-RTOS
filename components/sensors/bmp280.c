#include "bmp280.h"

#include <stdint.h>

#include "bsp.h"

#define TAG "BMP280"

static pressure_sensor_t pressure_sensor_instance;

void bmp280_init() {
  esp_err_t err;
  // use the "handheld device dynamic" optimal setting (see datasheet)
  uint8_t buf[2];

  const uint8_t reg_config_val = ((0x00 << 5) | (0x00 << 2)) & 0xFC;

  // send register number followed by its corresponding value
  buf[0] = REG_CONFIG;
  buf[1] = reg_config_val;
  err = i2c_master_write_to_device(I2C_MASTER_NUM, BMP280_SENSOR_ADDRESS, &buf, 2, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "bmp280 error %d", err);
    while (1)
      ;
  }

  // osrs_t x2 osrs_p x1, normal mode operation
  const uint8_t reg_ctrl_meas_val = (0x2 << 5) | (0x1 << 2) | (0x03);
  buf[0] = REG_CTRL_MEAS;
  buf[1] = reg_ctrl_meas_val;
  err = i2c_master_write_to_device(I2C_MASTER_NUM, BMP280_SENSOR_ADDRESS, &buf, 2, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "bmp280 error %d", err);
    while (1)
      ;
  }

  pressure_sensor_instance.init_altitude = 0;
  bmp280_get_calib_params(&pressure_sensor_instance.params);

  bmp280_update();
  pressure_sensor_instance.init_altitude = pressure_sensor_instance.altitude;
}

void bmp280_read_raw(int32_t* temp, int32_t* pressure) {
  // BMP280 data registers are auto-incrementing and we have 3 temperature and
  // pressure registers each, so we start at 0xF7 and read 6 bytes to 0xFC
  // note: normal mode does not require further ctrl_meas and config register writes

  uint8_t buf[6];
  uint8_t reg = REG_PRESSURE_MSB;
  esp_err_t err = i2c_master_write_read_device(I2C_MASTER_NUM, BMP280_SENSOR_ADDRESS, &reg, 1, buf, 6, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);

  // store the 20 bit read in a 32 bit signed integer for conversion
  *pressure = (buf[0] << 12) | (buf[1] << 4) | (buf[2] >> 4);
  *temp = (buf[3] << 12) | (buf[4] << 4) | (buf[5] >> 4);
}

void bmp280_reset() {
  // reset the device with the power-on-reset procedure
  uint8_t buf[2] = {REG_RESET, 0xB6};
  i2c_master_write_to_device(I2C_MASTER_NUM, BMP280_SENSOR_ADDRESS, &buf, 2, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

// intermediate function that calculates the fine resolution temperature
// used for both pressure and temperature conversions
int32_t bmp280_convert(int32_t temp, struct bmp280_calib_param* params) {
  // use the 32-bit fixed point compensation implementation given in the
  // datasheet

  int32_t var1, var2;
  var1 = ((((temp >> 3) - ((int32_t)params->dig_t1 << 1))) * ((int32_t)params->dig_t2)) >> 11;
  var2 = (((((temp >> 4) - ((int32_t)params->dig_t1)) * ((temp >> 4) - ((int32_t)params->dig_t1))) >> 12) * ((int32_t)params->dig_t3)) >> 14;
  return var1 + var2;
}

int32_t bmp280_convert_temp(int32_t temp, struct bmp280_calib_param* params) {
  // uses the BMP280 calibration parameters to compensate the temperature value read from its registers
  int32_t t_fine = bmp280_convert(temp, params);
  return (t_fine * 5 + 128) >> 8;
}

int32_t bmp280_convert_pressure(int32_t pressure, int32_t temp, struct bmp280_calib_param* params) {
  // uses the BMP280 calibration parameters to compensate the pressure value read from its registers

  int32_t t_fine = bmp280_convert(temp, params);

  int32_t var1, var2;
  uint32_t converted = 0.0;
  var1 = (((int32_t)t_fine) >> 1) - (int32_t)64000;
  var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((int32_t)params->dig_p6);
  var2 += ((var1 * ((int32_t)params->dig_p5)) << 1);
  var2 = (var2 >> 2) + (((int32_t)params->dig_p4) << 16);
  var1 = (((params->dig_p3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + ((((int32_t)params->dig_p2) * var1) >> 1)) >> 18;
  var1 = ((((32768 + var1)) * ((int32_t)params->dig_p1)) >> 15);
  if (var1 == 0) {
    return 0;  // avoid exception caused by division by zero
  }
  converted = (((uint32_t)(((int32_t)1048576) - pressure) - (var2 >> 12))) * 3125;
  if (converted < 0x80000000) {
    converted = (converted << 1) / ((uint32_t)var1);
  } else {
    converted = (converted / (uint32_t)var1) * 2;
  }
  var1 = (((int32_t)params->dig_p9) * ((int32_t)(((converted >> 3) * (converted >> 3)) >> 13))) >> 12;
  var2 = (((int32_t)(converted >> 2)) * ((int32_t)params->dig_p8)) >> 13;
  converted = (uint32_t)((int32_t)converted + ((var1 + var2 + params->dig_p7) >> 4));
  return converted;
}

void bmp280_get_calib_params(struct bmp280_calib_param* params) {
  // raw temp and pressure values need to be calibrated according to
  // parameters generated during the manufacturing of the sensor
  // there are 3 temperature params, and 9 pressure params, each with a LSB
  // and MSB register, so we read from 24 registers

  uint8_t buf[NUM_CALIB_PARAMS] = {0};
  uint8_t reg = REG_DIG_T1_LSB;
  i2c_master_write_read_device(I2C_MASTER_NUM, BMP280_SENSOR_ADDRESS, &reg, 1, buf, NUM_CALIB_PARAMS, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);

  // store these in a struct for later use
  params->dig_t1 = (uint16_t)(buf[1] << 8) | buf[0];
  params->dig_t2 = (int16_t)(buf[3] << 8) | buf[2];
  params->dig_t3 = (int16_t)(buf[5] << 8) | buf[4];

  params->dig_p1 = (uint16_t)(buf[7] << 8) | buf[6];
  params->dig_p2 = (int16_t)(buf[9] << 8) | buf[8];
  params->dig_p3 = (int16_t)(buf[11] << 8) | buf[10];
  params->dig_p4 = (int16_t)(buf[13] << 8) | buf[12];
  params->dig_p5 = (int16_t)(buf[15] << 8) | buf[14];
  params->dig_p6 = (int16_t)(buf[17] << 8) | buf[16];
  params->dig_p7 = (int16_t)(buf[19] << 8) | buf[18];
  params->dig_p8 = (int16_t)(buf[21] << 8) | buf[20];
  params->dig_p9 = (int16_t)(buf[23] << 8) | buf[22];
}

static inline float pressure2altitude(int32_t pressure) {
  float mbar = (float)pressure / 100.0;
  return 145366.45 * (1 - pow(mbar / 1013.25, 0.190284)) * 0.3084;
}

void bmp280_update() {
  static int32_t raw_temperature, raw_pressure;
  static float last_altitude_iir = 0;
  static float last_altitude_lpf = 0;
  static float last_velocity_lpf = 0;
  static float last_velocity_iir = 0;
  static float iir_altitude = 0;
  bmp280_read_raw(&raw_temperature, &raw_pressure);
  pressure_sensor_instance.temperature = bmp280_convert_temp(raw_temperature, &pressure_sensor_instance.params);
  pressure_sensor_instance.pressure = bmp280_convert_pressure(raw_pressure, raw_temperature, &pressure_sensor_instance.params);
  pressure_sensor_instance.altitude = pressure2altitude(pressure_sensor_instance.pressure);
  pressure_sensor_instance.last_update = bsp_current_time();

  pressure_sensor_instance.relative_altitude = pressure_sensor_instance.altitude - pressure_sensor_instance.init_altitude;
  pressure_sensor_instance.relative_altitude = lpf(pressure_sensor_instance.relative_altitude, last_altitude_lpf, 20, 100);
  last_altitude_lpf = pressure_sensor_instance.relative_altitude;

  iir_altitude = iir_1st(pressure_sensor_instance.relative_altitude, last_altitude_iir, 0.98);
  pressure_sensor_instance.velocity = 100 * (iir_altitude - last_altitude_iir);
  pressure_sensor_instance.velocity = lpf(pressure_sensor_instance.velocity, last_velocity_lpf, 10, 100);
  last_velocity_lpf = pressure_sensor_instance.velocity;
  pressure_sensor_instance.velocity = iir_1st(pressure_sensor_instance.velocity, last_velocity_iir, 0.8);

  last_altitude_iir = iir_altitude;
  last_velocity_iir = pressure_sensor_instance.velocity;
}

pressure_sensor_t* bmp_fetch() {
  return &pressure_sensor_instance;
}
