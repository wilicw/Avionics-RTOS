#include "sensors.h"

void sensors_task(void) {
  static gps_t* instance = NULL;
  gps_init();
  for (;;) {
    k_sleep(K_MSEC(1000));
    instance = gps_fetch();
    printk("%d,%d\n", instance->latitude, instance->longitude);
  }
}
