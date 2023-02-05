#include "gps.h"

LOG_MODULE_REGISTER(gps, LOG_LEVEL_DBG);

static const struct device *gps_uart_dev = DEVICE_DT_GET(DEFAULT_GPS_NODE);
static gps_t gps_instance;

static inline void gnss_encode(uint8_t *raw) {
  /* Only prase GGA message */
  if (strstr(raw, "GGA") == NULL) return;
  LOG_DBG("%s", raw);

  static nmea_t parsed;
  parsed.len = 0;

  parsed.field[parsed.len] = raw;
  parsed.len += 1;
  while ((raw = strchr(raw, ',')) != NULL) {
    *raw = 0;
    raw += 1;
    parsed.field[parsed.len] = raw;
    parsed.len += 1;
  }

  if (strcmp(&parsed.field[0][3], "GGA") == 0) {
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

    /* Set ready to true */
    gps_instance.ready = CHAR2INT(parsed.field[6][0]);

    /* Number of satellites */
    gps_instance.satellites = CHAR2INT(parsed.field[7][0]) * 10 + CHAR2INT(parsed.field[7][1]);

    LOG_DBG("TIME : %d %d %d", gps_instance.time.hour, gps_instance.time.minute, gps_instance.time.second);
    LOG_DBG("LAT  : %d", gps_instance.latitude);
    LOG_DBG("LONG : %d", gps_instance.longitude);
    LOG_DBG("NUM  : %d", gps_instance.satellites);
  }
}

void gps_init() {
  /* Init gps instance object */
  gps_instance.ready = 0;

  if (!device_is_ready(gps_uart_dev)) {
    LOG_ERR("UART2 is not ready");
    return;
  }

  /* Enable GPS RX IRQ */
  uart_irq_callback_set(gps_uart_dev, gps_isr_callback);
  uart_irq_rx_enable(gps_uart_dev);
}

void gps_isr_callback() {
  static uint8_t uart_buffer[BUFFER_MAX] = {0};
  static size_t ptr;
  static uint8_t len;
  uart_irq_update(gps_uart_dev);
  if (uart_irq_rx_ready(gps_uart_dev)) {
    while ((len = uart_fifo_read(gps_uart_dev, uart_buffer + ptr, BUFFER_BLOCK)) == BUFFER_BLOCK) {
      ptr += len;
      uart_buffer[ptr] = 0;
      if (uart_buffer[0] == '$' && uart_buffer[ptr - 1] == '\n') {
        uart_buffer[ptr - 2] = 0;
        gnss_encode(uart_buffer);
        ptr = 0;
      }
    }
  }
}

gps_t *gps_fetch() {
  return &gps_instance;
}
