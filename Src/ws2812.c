//
// Created by William Chang on 2023/2/21.
//

#include <ws2812.h>

static uint8_t buffer[124];

void ws2812_set(SPI_HandleTypeDef *hspi, uint8_t red, uint8_t green, uint8_t blue, uint8_t dma) {
  memset(buffer, 0, sizeof(buffer));
  WS2812_COLOR(buffer + 100, red, green, blue);
  HAL_SPI_Transmit_DMA(hspi, buffer, sizeof(buffer));
}
