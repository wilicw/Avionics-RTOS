//
// Created by William Chang on 2023/2/22.
//

#ifndef AVIONICS_SLAVE_MCU_BUZZER_H
#define AVIONICS_SLAVE_MCU_BUZZER_H

#include <stdint.h>
#include "stm32g0xx_hal.h"

typedef struct {
  uint8_t enabled;
  uint32_t inter_counter;
  uint32_t inter_div;
  uint32_t div;
  GPIO_TypeDef *port;
  uint16_t pin;
  uint32_t duration;
} buzz_t;

void buzzer_tone(buzz_t *, uint16_t, uint32_t);

void buzzer_enable(buzz_t *);

void buzzer_disable(buzz_t *);

void buzzer_handler(buzz_t *);

#endif //AVIONICS_SLAVE_MCU_BUZZER_H
