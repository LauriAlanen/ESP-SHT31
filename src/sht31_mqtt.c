#include "../include/sht31_mqtt.h"

//const uint8_t cert_pem_start[] asm("_binary_cert_pem_start");
//const uint8_t cert_pem_end[] asm("_binary_cert_pem_end");

void mqtt_publish_sensor_data(esp_mqtt_client_handle_t client, uint8_t *sht31_response)
{
    mqtt_publish_semaphore = xSemaphoreCreateBinary();
    char mqtt_payload[5];
    int temperature_id, humidity_id;
    
    snprintf(mqtt_payload, 5, "%.2f", sht31_convert_humidity(sht31_response));
    temperature_id = esp_mqtt_client_publish(client, CONFIG_MQTT_HUMIDITY_TOPIC, mqtt_payload, 5, CONFIG_MQTT_QOS, 0);

    if (xSemaphoreTake(mqtt_publish_semaphore, pdMS_TO_TICKS(5000)) == pdTRUE)
    {
        ESP_LOGI(MQTT_TAG, "Successfully taken semaphore");
    }

    else
    {
        ESP_LOGE(MQTT_TAG, "Timeout while waiting for temperature acknowledgment!");
    }
    
    snprintf(mqtt_payload, 5, "%.2f", sht31_convert_temperature(sht31_response));
    humidity_id = esp_mqtt_client_publish(client, CONFIG_MQTT_TEMPERATURE_TOPIC, mqtt_payload, 5, CONFIG_MQTT_QOS, 0);

    if (xSemaphoreTake(mqtt_publish_semaphore, pdMS_TO_TICKS(5000)) == pdTRUE)
    {
        ESP_LOGI(MQTT_TAG, "Successfully taken semaphore");
    }

    else
    {
        ESP_LOGE(MQTT_TAG, "Timeout while waiting for humidity acknowledgment!");
    }
}

void mqtt_event_handler(void *args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t) event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(MQTT_TAG, "Succesfully connected to the broker!");
        break;

    case MQTT_EVENT_BEFORE_CONNECT:
        ESP_LOGI(MQTT_TAG, "Connecting to the broker...");
        break;
    
    case MQTT_EVENT_PUBLISHED:
        xSemaphoreGive(mqtt_publish_semaphore);
        ESP_LOGI(MQTT_TAG, "Published a message with id : %d", event->msg_id);
        break;

    default:
        ESP_LOGI(MQTT_TAG, "Unexpected event id with id %d", (int)event_id);
        break;
    }
}

esp_mqtt_client_handle_t mqtt_app_start(void)
{
    ESP_LOGI(MQTT_TAG, "Attempting to start MQTT");
    esp_mqtt_client_config_t config = {
        .broker = {
            .address.uri = CONFIG_MQTT_BROKER_URI,
            //.verification.certificate = (const char*)cert_pem_start, 
        },
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&config);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
    
    return client;
}

