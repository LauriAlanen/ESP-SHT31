#include "../include/sht31_wifi.h"
#include "../include/sht31_mqtt.h"
#include "../include/sht31_i2c.h"
#include <esp_sleep.h>

#define SLEEP_TIME 1800

int app_main(void)
{
    esp_err_t status = WIFI_FAILURE;
    esp_err_t nvs_init_result = nvs_flash_init();    
    esp_mqtt_client_handle_t client;
    
    uint8_t sht31_response[6];

    if (nvs_init_result == ESP_ERR_NVS_NO_FREE_PAGES || nvs_init_result == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        nvs_init_result = nvs_flash_init();
    }

    ESP_ERROR_CHECK(nvs_init_result);

    while (1)
    {
        gpio_reset_pin(GPIO_NUM_2); 
        gpio_set_direction(GPIO_NUM_2, GPIO_MODE_DEF_OUTPUT);

        gpio_set_level(GPIO_NUM_2, 1);
        printf("Woke up from sleep\n");
        status = wifi_connect();

        if (WIFI_SUCCESS != status)
        {
            ESP_LOGE(WIFI_TAG, "Failed to connect to the Access Point in main loop.");
        }

        else
        {
            client = mqtt_app_start();
            i2c_master_init();
            
            ESP_ERROR_CHECK(sht31_start_measurement());
            ESP_ERROR_CHECK(sht31_read_measurement(sht31_response));
            mqtt_publish_sensor_data(client, sht31_response);

            printf("Entering sleep\n");
            gpio_set_level(GPIO_NUM_2, 0);
            esp_deep_sleep(SLEEP_TIME * 1000000ULL);
        }
    }
    
    return 0;   
}   