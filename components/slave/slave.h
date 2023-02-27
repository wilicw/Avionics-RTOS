#ifndef __SLAVE_H__
#define __SLAVE_H__

#include <driver/i2c.h>
#include <stdint.h>

#include "bsp.h"

#define SLAVE_ADDR 0x17

void slave_reset();
void buzzer(uint32_t);
void rgb(uint8_t, uint8_t, uint8_t);
void servo(uint8_t, int16_t);

#endif
