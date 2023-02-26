//
// Created by William Chang on 2023/2/22.
//

#include "buzzer.h"

void buzzer_tone(buzz_t *buzz, uint16_t tone, uint32_t duration) {
  buzz->duration = duration * buzz->inter_div / 1000 / 2;
  buzz->div = buzz->inter_div / tone;
}

void buzzer_enable(buzz_t *buzz) {
  buzz->enabled = 1;
}

void buzzer_disable(buzz_t *buzz) {
  buzz->enabled = 0;
}

void buzzer_handler(buzz_t *buzz) {
  if (buzz->enabled && buzz->duration) {
    if (buzz->inter_counter % buzz->div == 0) {
      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0);
    }
    buzz->inter_counter = (buzz->inter_counter + 1) % buzz->div;
    buzz->duration--;
  } else {
    buzz->inter_counter = 0;
    HAL_GPIO_WritePin(buzz->port, buzz->pin, 0);
  }
}
