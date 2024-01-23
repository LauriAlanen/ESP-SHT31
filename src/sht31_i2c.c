#include "../include/sht31_i2c.h"

void i2c_master_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_PIN,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,  
    };

    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

esp_err_t sht31_start_measurement()
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    esp_err_t result;

    uint8_t addr = (SHT31_SENSOR_ADDRESS_PRIMARY << 1) | I2C_MASTER_WRITE;
    uint8_t command[] = {addr, 0x2C, 0x06};

    i2c_master_start(cmd);
    i2c_master_write(cmd, command, 3, true);
    i2c_master_stop(cmd);

    result = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    return result;
}

esp_err_t sht31_read_measurement(uint8_t *sht31_response)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    esp_err_t result;

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, SHT31_SENSOR_ADDRESS_PRIMARY << 1 | I2C_MASTER_READ, true);
    vTaskDelay(100/portTICK_PERIOD_MS);
    i2c_master_read(cmd, sht31_response, 6, I2C_MASTER_LAST_NACK);

    i2c_master_stop(cmd);
    result = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_MASTER_TIMEOUT_MS/portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    return result;
}

float sht31_convert_temperature(uint8_t* sht31_response) 
{
    uint16_t raw_temperature = (sht31_response[0] << 8) | sht31_response[1];
    return 175.0f * (float)raw_temperature / 65535.0f - 45.0f;
}

float sht31_convert_humidity(uint8_t* sht31_response) 
{
    uint16_t raw_humidity = (sht31_response[3] << 8) | sht31_response[4];
    return 100.0f * (float)raw_humidity / 65535.0f;
}