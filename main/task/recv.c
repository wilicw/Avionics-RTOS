#include "recv.h"

#define TAG "RECV"

#define RECV_LEN 255

void recv_task(void* args) {
  uint8_t data[RECV_LEN];
  size_t len;
  for (;;) {
    if ((len = LoRaReceive(&data[0], RECV_LEN)) > 0) {
      ESP_LOG_BUFFER_HEX(TAG, &data[0], len);
    }
  }
}
