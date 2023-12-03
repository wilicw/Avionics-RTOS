#ifndef __GPS_H__
#define __GPS_H__

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdint.h>
#include <string.h>

#include "bsp.h"

#define CHAR2INT(x) ((x) - '0')

typedef struct {
  uint8_t* field[25];
  uint8_t len;
} nmea_t;

typedef struct {
  /* Position information
   * Stored in minute format, positive for North/East
   * Example:
   *  Degree format: 25.052435
   *  Minute format: 15031461
   * */
  int32_t latitude;
  int32_t longitude;
  float altitude;
  /* UTC Time */
  struct {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
  } time;
  uint8_t satellites;
  uint8_t ready;
} gps_t;

void gps_init();
void gps_parse_task();
gps_t* gps_fetch();

#endif
