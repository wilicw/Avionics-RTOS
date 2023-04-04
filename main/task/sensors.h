#ifndef __SENSORS_T__
#define __SENSORS_T__

#include "MadgwickAHRS.h"
#include "bmp280.h"
#include "calibrate.h"
#include "gps.h"
#include "mpu9250.h"
#include "slave.h"

void sensors_task(void *);

#endif
