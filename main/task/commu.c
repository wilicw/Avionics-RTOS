#include "commu.h"

#define TAG "COMMU"

void commu_task(void* args) {
  const static uint32_t frequencyInHz = 433E6;
  const static int8_t txPowerInDbm = 22;
  const static float tcxoVoltage = 3.3;
  const static bool useRegulatorLDO = true;
  LoRaInit();
  int ret = LoRaBegin(frequencyInHz, txPowerInDbm, tcxoVoltage, useRegulatorLDO);
  ESP_LOGI(TAG, "LoRaBegin=%d", ret);
  if (ret != 0) {
    ESP_LOGE(pcTaskGetName(NULL), "Does not recognize the module");
    while (1) {
      vTaskDelay(1);
    }
  }

  uint8_t spreadingFactor = 7;
  uint8_t bandwidth = 4;
  uint8_t codingRate = 1;
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

  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
