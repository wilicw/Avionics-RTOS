//
// Created by William Chang on 2023/2/22.
//

#ifndef AVIONICS_SLAVE_MCU_SERVO_H
#define AVIONICS_SLAVE_MCU_SERVO_H

#include <stdint.h>
#include "stm32g0xx_hal.h"

#define SERVO_PERIOD 200 // 0.1ms
#define SERVO_MIN 5 // 0.1ms
#define SERVO_MAX 25 // 0.1ms

typedef struct {
  uint8_t enabled;
  GPIO_TypeDef *port;
  uint16_t pin;
  uint32_t angle;
  int16_t _angle;
  uint32_t inter_div;
  uint32_t inter_counter;
} servo_t;

void servo_set(servo_t *, int16_t);

void servo_enable(servo_t *);

void servo_disable(servo_t *);

void servo_handler(servo_t *);

#endif //AVIONICS_SLAVE_MCU_SERVO_H
