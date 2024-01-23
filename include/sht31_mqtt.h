#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "sht31_i2c.h"

static const char *MQTT_TAG = "MQTT";
static SemaphoreHandle_t mqtt_publish_semaphore;

extern esp_mqtt_client_handle_t mqtt_app_start(void);
extern void mqtt_event_handler(void *args, esp_event_base_t base, int32_t event_id, void *event_data);
extern void mqtt_publish_sensor_data(esp_mqtt_client_handle_t client, uint8_t *sht31_response);