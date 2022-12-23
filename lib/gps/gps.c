#include "gps.h"

static const struct device *gps_uart_dev = DEVICE_DT_GET(DT_ALIAS(gps));

void gps_thread() {
  static size_t ptr = 0;
  static uint8_t uart_buffer[BUFFERSIZE] = {0};
  static uint8_t c;

  if (!device_is_ready(gps_uart_dev)) {
    printk("UART2 is not ready\n");
    return;
  }

  for (;;) {
    while (!uart_poll_in(gps_uart_dev, &c)) {
      if (c == '\n') {
        uart_buffer[ptr] = 0;
        ptr = 0;
        if (strstr(uart_buffer, "$GPGGA"))
          printk("%s\n", uart_buffer);
      } else {
        uart_buffer[ptr++] = c;
      }
    }
    k_msleep(100);
  }
}
