/**
 * Copyright (c) 2021 Raspberry Pi (Trading) Ltd.
 * from https://github.com/raspberrypi/pico-examples/blob/master/i2c/bmp280_i2c/bmp280_i2c.c
 * SPDX-License-Identifier: BSD-3-Clause
 * Modify By @wilicw
 **/

#ifndef __BMP_H__
#define __BMP_H__

#include <math.h>
#include <stdint.h>

#include "bsp.h"

#define _u(x) (x)
// device has default bus address of 0x76
#define BMP280_SENSOR_ADDRESS 0x76

// hardware registers
#define REG_CONFIG _u(0xF5)
#define REG_CTRL_MEAS _u(0xF4)
#define REG_RESET _u(0xE0)

#define REG_TEMP_XLSB _u(0xFC)
#define REG_TEMP_LSB _u(0xFB)
#define REG_TEMP_MSB _u(0xFA)

#define REG_PRESSURE_XLSB _u(0xF9)
#define REG_PRESSURE_LSB _u(0xF8)
#define REG_PRESSURE_MSB _u(0xF7)

// calibration registers
#define REG_DIG_T1_LSB _u(0x88)
#define REG_DIG_T1_MSB _u(0x89)
#define REG_DIG_T2_LSB _u(0x8A)
#define REG_DIG_T2_MSB _u(0x8B)
#define REG_DIG_T3_LSB _u(0x8C)
#define REG_DIG_T3_MSB _u(0x8D)
#define REG_DIG_P1_LSB _u(0x8E)
#define REG_DIG_P1_MSB _u(0x8F)
#define REG_DIG_P2_LSB _u(0x90)
#define REG_DIG_P2_MSB _u(0x91)
#define REG_DIG_P3_LSB _u(0x92)
#define REG_DIG_P3_MSB _u(0x93)
#define REG_DIG_P4_LSB _u(0x94)
#define REG_DIG_P4_MSB _u(0x95)
#define REG_DIG_P5_LSB _u(0x96)
#define REG_DIG_P5_MSB _u(0x97)
#define REG_DIG_P6_LSB _u(0x98)
#define REG_DIG_P6_MSB _u(0x99)
#define REG_DIG_P7_LSB _u(0x9A)
#define REG_DIG_P7_MSB _u(0x9B)
#define REG_DIG_P8_LSB _u(0x9C)
#define REG_DIG_P8_MSB _u(0x9D)
#define REG_DIG_P9_LSB _u(0x9E)
#define REG_DIG_P9_MSB _u(0x9F)

// number of calibration registers to be read
#define NUM_CALIB_PARAMS 24

struct bmp280_calib_param {
  // temperature params
  uint16_t dig_t1;
  int16_t dig_t2;
  int16_t dig_t3;

  // pressure params
  uint16_t dig_p1;
  int16_t dig_p2;
  int16_t dig_p3;
  int16_t dig_p4;
  int16_t dig_p5;
  int16_t dig_p6;
  int16_t dig_p7;
  int16_t dig_p8;
  int16_t dig_p9;
};

typedef struct {
  int32_t temperature;
  int32_t pressure;
  float altitude;
  float relative_altitude;
  float velocity;
  float init_altitude;
  struct bmp280_calib_param params;
  uint32_t last_update;
} pressure_sensor_t;

void bmp280_init();
void bmp280_read_raw(int32_t *, int32_t *);
void bmp280_reset();
int32_t bmp280_convert(int32_t, struct bmp280_calib_param *);
int32_t bmp280_convert_temp(int32_t, struct bmp280_calib_param *);
int32_t bmp280_convert_pressure(int32_t, int32_t, struct bmp280_calib_param *);
void bmp280_get_calib_params(struct bmp280_calib_param *);
void bmp280_update();
pressure_sensor_t *bmp_fetch();

#endif
