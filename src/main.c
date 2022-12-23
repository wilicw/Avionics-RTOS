#include <gps/gps.h>
#include <zephyr/kernel.h>

#define STACKSIZE 1024
#define PRIORITY 7

K_THREAD_DEFINE(T_GPS, STACKSIZE, gps_thread, NULL, NULL, NULL, PRIORITY, 0, 0);
