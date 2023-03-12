#include "commu.h"

#define TAG "COMMU"

static SemaphoreHandle_t *spi_spinlock;

void tx_task(void *args) {
  static uint8_t data[128];
  for (;;) {
    if (xSemaphoreTake(*spi_spinlock, portMAX_DELAY) == pdTRUE) {
      LoRaSend(data, sizeof(data), SX126x_TXMODE_SYNC);
      xSemaphoreGive(*spi_spinlock);
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
  }
}

void rx_task(void *args) {
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void commu_task(void *args) {
  const static uint32_t frequencyInHz = 433E6;
  const static int8_t txPowerInDbm = 22;
  const static float tcxoVoltage = 3.3;
  const static bool useRegulatorLDO = true;
  spi_spinlock = fetch_spi_spinlock();

  if (xSemaphoreTake(*spi_spinlock, portMAX_DELAY) == pdTRUE) {
    LoRaInit();
    int ret = LoRaBegin(frequencyInHz, txPowerInDbm, tcxoVoltage, useRegulatorLDO);
    ESP_LOGI(TAG, "LoRaBegin=%d", ret);
    if (ret != 0) {
      ESP_LOGE(pcTaskGetName(NULL), "Does not recognize the module");
    }

    uint8_t spreadingFactor = 7;
    uint8_t bandwidth = SX126X_LORA_BW_125_0;
    uint8_t codingRate = SX126X_LORA_CR_4_5;
    uint16_t preambleLength = 8;
    uint8_t payloadLen = 0;
    bool crcOn = true;
    bool invertIrq = false;
#if CONFIF_ADVANCED
    spreadingFactor = CONFIG_SF_RATE;
    bandwidth = CONFIG_BANDWIDTH;
    codingRate = CONFIG_CODING_RATE;
#endif
    LoRaConfig(spreadingFactor, bandwidth, codingRate, preambleLength, payloadLen, crcOn, invertIrq);
    xSemaphoreGive(*spi_spinlock);
  }

  xTaskCreatePinnedToCore(tx_task, "commu_tx_task", 4096, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(rx_task, "commu_rx_task", 4096, NULL, 2, NULL, 0);

  vTaskDelete(NULL);
}
