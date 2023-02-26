//
// Created by William Chang on 2023/2/22.
//

#include "servo.h"

void servo_set(servo_t *servo, int16_t angle) {
  servo->_angle = angle;
}

void servo_enable(servo_t *servo) {
  servo->enabled = 1;
}

void servo_disable(servo_t *servo) {
  servo->enabled = 0;
}

void servo_handler(servo_t *servo) {
  if (servo->enabled) {
    if (servo->inter_counter < servo->angle) {
      HAL_GPIO_WritePin(servo->port, servo->pin, 1);
    } else {
      HAL_GPIO_WritePin(servo->port, servo->pin, 0);
    }
    servo->inter_counter = (servo->inter_counter + 1) % (SERVO_PERIOD * servo->inter_div / 10000);
    if (servo->inter_counter == 0) {
      servo->angle =
        (((float) (servo->_angle + 180)) * ((SERVO_MAX - SERVO_MIN) / 360.0f) + SERVO_MIN) * servo->inter_div / 10000;
    }
  } else {
    HAL_GPIO_WritePin(servo->port, servo->pin, 0);
    servo->inter_counter = 0;
  }
}

