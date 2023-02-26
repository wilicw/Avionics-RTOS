//
// Created by William Chang on 2023/2/21.
//

#ifndef AVIONICS_SLAVE_MCU_WS2812_H
#define AVIONICS_SLAVE_MCU_WS2812_H

#include <stdint.h>
#include <main.h>
#include <string.h>

#define WS2812_LIM 32
#define WS2812_0 0b10000000U
#define WS2812_1 0b11111000U
#define WS2812_COLOR(buf, R, G, B)                    \
  {                                                   \
    *((buf) + 0) = ((G)&0x80) ? WS2812_1 : WS2812_0;  \
    *((buf) + 1) = ((G)&0x40) ? WS2812_1 : WS2812_0;  \
    *((buf) + 2) = ((G)&0x20) ? WS2812_1 : WS2812_0;  \
    *((buf) + 3) = ((G)&0x10) ? WS2812_1 : WS2812_0;  \
    *((buf) + 4) = ((G)&0x08) ? WS2812_1 : WS2812_0;  \
    *((buf) + 5) = ((G)&0x04) ? WS2812_1 : WS2812_0;  \
    *((buf) + 6) = ((G)&0x02) ? WS2812_1 : WS2812_0;  \
    *((buf) + 7) = ((G)&0x01) ? WS2812_1 : WS2812_0;  \
    *((buf) + 8) = ((R)&0x80) ? WS2812_1 : WS2812_0;  \
    *((buf) + 9) = ((R)&0x40) ? WS2812_1 : WS2812_0;  \
    *((buf) + 10) = ((R)&0x20) ? WS2812_1 : WS2812_0; \
    *((buf) + 11) = ((R)&0x10) ? WS2812_1 : WS2812_0; \
    *((buf) + 12) = ((R)&0x08) ? WS2812_1 : WS2812_0; \
    *((buf) + 13) = ((R)&0x04) ? WS2812_1 : WS2812_0; \
    *((buf) + 14) = ((R)&0x02) ? WS2812_1 : WS2812_0; \
    *((buf) + 15) = ((R)&0x01) ? WS2812_1 : WS2812_0; \
    *((buf) + 16) = ((B)&0x80) ? WS2812_1 : WS2812_0; \
    *((buf) + 17) = ((B)&0x40) ? WS2812_1 : WS2812_0; \
    *((buf) + 18) = ((B)&0x20) ? WS2812_1 : WS2812_0; \
    *((buf) + 19) = ((B)&0x10) ? WS2812_1 : WS2812_0; \
    *((buf) + 20) = ((B)&0x08) ? WS2812_1 : WS2812_0; \
    *((buf) + 21) = ((B)&0x04) ? WS2812_1 : WS2812_0; \
    *((buf) + 22) = ((B)&0x02) ? WS2812_1 : WS2812_0; \
    *((buf) + 23) = ((B)&0x01) ? WS2812_1 : WS2812_0; \
  }

void ws2812_set(SPI_HandleTypeDef *, uint8_t, uint8_t, uint8_t, uint8_t);

#endif  // AVIONICS_SLAVE_MCU_WS2812_H
