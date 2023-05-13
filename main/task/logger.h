#ifndef __LOGGER_T__
#define __LOGGER_T__

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "gps.h"
#include "ra01s.h"
#include "storage.h"

void logger_task(void*);

#endif
