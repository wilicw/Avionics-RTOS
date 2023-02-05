#include <sensors.h>
#include <zephyr/kernel.h>

#define STACKSIZE 1024
#define PRIORITY 7

K_THREAD_DEFINE(T_LORA, STACKSIZE, sensors_task, NULL, NULL, NULL, PRIORITY, 0, 0);
