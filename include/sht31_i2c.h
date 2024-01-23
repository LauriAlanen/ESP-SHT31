#pragma once

#include "driver/gpio.h"
#include "driver/i2c.h"

#define SHT31_SENSOR_ADDRESS_PRIMARY 0x45

#define I2C_MASTER_NUM 0
#define I2C_MASTER_SDA_PIN GPIO_NUM_21
#define I2C_MASTER_SCL_PIN GPIO_NUM_22
#define I2C_MASTER_FREQ_HZ 50000
#define I2C_MASTER_TX_BUF_DISABLE 0
#define I2C_MASTER_RX_BUF_DISABLE 0
#define I2C_MASTER_TIMEOUT_MS 1000

extern void i2c_master_init(void);
extern esp_err_t sht31_start_measurement();
extern esp_err_t sht31_read_measurement(uint8_t *sht31_response);
extern float sht31_convert_temperature(uint8_t* sht31_response);
extern float sht31_convert_humidity(uint8_t* sht31_response);

