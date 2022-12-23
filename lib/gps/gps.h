#ifndef __GPS_H__
#define __GPS_H__

#include <stdint.h>
#include <string.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>

#define BUFFERSIZE 1024

void gps_thread(void);

#endif
