#include "commu.h"

static uint8_t buffer[128];

void buffer_init() {
  return;
}

uint8_t* buffer_fetch() {
  return &buffer[0];
}

size_t buffer_len() {
  return sizeof(buffer);
}
