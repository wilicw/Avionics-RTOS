#include "slave.h"

#define TAG "SLAVE"

static uint8_t i2c_buffer[5];

void execute() {
  i2c_master_write_to_device(I2C_MASTER_NUM, SLAVE_ADDR, i2c_buffer, sizeof(i2c_buffer), pdMS_TO_TICKS(100));
  vTaskDelay(pdMS_TO_TICKS(10));
}

void slave_reset() {
  i2c_buffer[0] = 0;
  execute();
}

void rgb(uint8_t red, uint8_t green, uint8_t blue) {
  i2c_buffer[0] = 0x1;
  i2c_buffer[1] = red;
  i2c_buffer[2] = green;
  i2c_buffer[3] = blue;
  execute();
}

void buzzer(uint32_t duration) {
  i2c_buffer[0] = 0x2;
  i2c_buffer[1] = duration >> 24;
  i2c_buffer[2] = duration >> 16;
  i2c_buffer[3] = duration >> 8;
  i2c_buffer[4] = duration;
  execute();
}

void servo(uint8_t id, int16_t ang) {
  i2c_buffer[0] = 0x2;
  i2c_buffer[1] = ang < 0 ? 1 : 0;
  i2c_buffer[2] = ang < 0 ? -ang : ang;
  execute();
}
