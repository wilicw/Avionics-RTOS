#include <zephyr/kernel.h>

void main(void) {
  for (;;) {
    printk("Meow\n");
    k_msleep(1000);
  }
}
