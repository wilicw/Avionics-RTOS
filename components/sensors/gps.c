#include "gps.h"

#define TAG "GPS"

static gps_t gps_instance;

void gps_init() {
  xTaskCreate(gps_parse_task, "gps_parse_task", 8192, NULL, 12, NULL);
}

static inline void gps_parser(uint8_t *raw) {
  // printf("%s", raw);
  /* Only prase GGA message */
  if (strstr((char *)raw, "GGA") == NULL) return;
  static nmea_t parsed;
  parsed.len = 0;
  parsed.field[parsed.len] = raw;
  parsed.len += 1;
  while ((raw = (uint8_t *)strchr((char *)raw, ',')) != NULL) {
    *raw = 0;
    raw += 1;
    parsed.field[parsed.len] = raw;
    parsed.len += 1;
  }

  if (strcmp((char *)&parsed.field[0][3], "GGA") == 0) {
    /* GGA Format
     * $--GGA,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,M,<11>,M,<13>,<14>*xx
     *  <1>: UTC Time: hhmmss.sss
     *  <2>: latitude: ddmm.mmmm(m)
     *  <3>: latitude direction: N+/S-
     *  <4>: longitude: ddmm.mmmm(m)
     *  <5>: longitude direction: E+/W-
     *  <6>: status: ref: https://docs.novatel.com/OEM7/Content/Logs/GPGGA.htm#GPSQualityIndicators
     *  <7>: number of satellites
     *  <8>: horizontal precision
     *  <9>: altitude
     *    M: unit of altitude (M for metres)
     * <11>: undulation: geoid undulation in WGS84
     *    M: unit of undulation (M for metres)
     * <13>: age of differential corrections
     * <14>: differential reference station ID
     *  */

    /* UTC time */
    gps_instance.time.hour = CHAR2INT(parsed.field[1][0]) * 10 + CHAR2INT(parsed.field[1][1]);
    gps_instance.time.minute = CHAR2INT(parsed.field[1][2]) * 10 + CHAR2INT(parsed.field[1][3]);
    gps_instance.time.second = CHAR2INT(parsed.field[1][4]) * 10 + CHAR2INT(parsed.field[1][5]);

    /* latitude: Parsed string to minutes format */
    if (parsed.field[2][0] != 0) {
      gps_instance.latitude =
          (CHAR2INT(parsed.field[2][0]) * 10 +
           CHAR2INT(parsed.field[2][1])) *
              600000 +
          CHAR2INT(parsed.field[2][2]) * 100000 +
          CHAR2INT(parsed.field[2][3]) * 10000 +
          CHAR2INT(parsed.field[2][5]) * 1000 +
          CHAR2INT(parsed.field[2][6]) * 100 +
          CHAR2INT(parsed.field[2][7]) * 10 +
          CHAR2INT(parsed.field[2][8]);
      gps_instance.latitude *= parsed.field[3][0] == 'N' ? 1 : -1;
    }

    /* longitude: Parsed string to minutes format */
    if (parsed.field[4][0] != 0) {
      gps_instance.longitude =
          (CHAR2INT(parsed.field[4][0]) * 100 +
           CHAR2INT(parsed.field[4][1]) * 10 +
           CHAR2INT(parsed.field[4][2])) *
              600000 +
          CHAR2INT(parsed.field[4][3]) * 100000 +
          CHAR2INT(parsed.field[4][4]) * 10000 +
          CHAR2INT(parsed.field[4][6]) * 1000 +
          CHAR2INT(parsed.field[4][7]) * 100 +
          CHAR2INT(parsed.field[4][8]) * 10 +
          CHAR2INT(parsed.field[4][9]);
      gps_instance.longitude *= parsed.field[5][0] == 'E' ? 1 : -1;
    }

    gps_instance.altitude = strtof((char *)parsed.field[9], NULL);

    /* Set ready to true */
    gps_instance.ready = CHAR2INT(parsed.field[6][0]);

    /* Number of satellites */
    gps_instance.satellites = CHAR2INT(parsed.field[7][0]) * 10 + CHAR2INT(parsed.field[7][1]);
  }
}

void gps_parse_task() {
  static uart_event_t event;
  static uint8_t buffer[256];
  static QueueHandle_t *uart_queue;
  static int8_t pos;

  uart_queue = fetch_uart_queue();
  esp_log_level_set(TAG, ESP_LOG_INFO);
  while (1) {
    if (xQueueReceive(*uart_queue, &event, pdMS_TO_TICKS(100))) {
      switch (event.type) {
        case UART_DATA:
          break;
        case UART_FIFO_OVF:
          ESP_LOGW(TAG, "HW FIFO Overflow");
          uart_flush(GPS_UART_NUM);
          xQueueReset(*uart_queue);
          break;
        case UART_BUFFER_FULL:
          ESP_LOGW(TAG, "Ring Buffer Full");
          uart_flush(GPS_UART_NUM);
          xQueueReset(*uart_queue);
          break;
        case UART_BREAK:
          ESP_LOGW(TAG, "Rx Break");
          break;
        case UART_PARITY_ERR:
          ESP_LOGE(TAG, "Parity Error");
          break;
        case UART_FRAME_ERR:
          ESP_LOGE(TAG, "Frame Error");
          break;
        case UART_PATTERN_DET: {
          pos = uart_pattern_pop_pos(GPS_UART_NUM);
          if (pos != -1) {
            int read_len = uart_read_bytes(GPS_UART_NUM, buffer, pos + 1, 100 / portTICK_PERIOD_MS);
            buffer[read_len] = '\0';
            gps_parser((uint8_t *)buffer);
          } else {
            ESP_LOGW(TAG, "Pattern Queue Size too small");
            uart_flush_input(GPS_UART_NUM);
          }
          break;
        }
        default:
          ESP_LOGW(TAG, "unknown uart event type: %d", event.type);
          break;
      }
    }
  }
}

gps_t *gps_fetch() {
  return &gps_instance;
}
