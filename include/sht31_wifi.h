#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "esp_wifi.h"
#include "esp_log.h"

#define WIFI_SUCCESS 1 << 0
#define WIFI_FAILURE 1 << 1
#define TCP_SUCCESS 1 << 0
#define TCP_FAILURE 1 << 1
#define MAX_FAILURES 10

static EventGroupHandle_t wifi_event_group;
static int retry_counter = 0;
static const char *WIFI_TAG = "WIFI";

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
static void ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
esp_err_t wifi_start(void);
esp_err_t wifi_connect();
