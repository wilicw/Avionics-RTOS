#ifndef __BSP_H__
#define __BSP_H__

#include <driver/gpio.h>
#include <driver/i2c.h>
#include <driver/spi_master.h>
#include <driver/uart.h>
#include <esp_log.h>
#include <esp_vfs_fat.h>
#include <math.h>
#include <sdmmc_cmd.h>
#include <stdint.h>
#include <string.h>

#define GPIO_FIRE_1 CONFIG_FIRE_1
#define GPIO_FIRE_2 CONFIG_FIRE_2

#define I2C_MASTER_SCL_IO CONFIG_I2C_MASTER_SCL /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO CONFIG_I2C_MASTER_SDA /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM 0                        /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ 4E5                  /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0             /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0             /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS 1000

#define GPS_UART_NUM UART_NUM_2

#define SPI_FREQ_HZ 1E6
#define LORA_SPI_HOST SPI2_HOST
#define SD_SPI_HOST SPI3_HOST

#define SD_MOUNT "/sd"

/**
 * @brief GPIO output initialization
 */
void gpio_init();

/**
 * @brief i2c master initialization
 */
void i2c_init();

/**
 * @brief UART initialization
 */
void uart_init();

/**
 * @brief spi initialization
 */
void spi_init(spi_host_device_t, uint32_t, uint32_t, uint32_t);
esp_err_t sd_init();

float lpf(float, float, float, float);
float iir_1st(float, float, float);

uint32_t bsp_current_time();

QueueHandle_t* fetch_uart_queue();
#endif
