#ifndef __GPS_L__
#define __GPS_L__

#include <stdint.h>
#include <string.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#define BUFFER_MAX 256
#define BUFFER_BLOCK 1
#define DEFAULT_GPS_NODE DT_ALIAS(gps)

#define CHAR2INT(x) ((x) - '0')

BUILD_ASSERT(DT_NODE_HAS_STATUS(DEFAULT_GPS_NODE, okay), "No default GPS module specified in DT");

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
void gps_isr_callback();
gps_t* gps_fetch();

#endif
