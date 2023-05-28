#ifndef __LOGGER_T__
#define __LOGGER_T__

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include <stdint.h>

#include "bsp.h"
#include "commu.h"
#include "driver/gpio.h"
#include "gps.h"
#include "ra01s.h"
#include "storage.h"

void logger_task(void*);

#endif
