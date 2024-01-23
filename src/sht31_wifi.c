#include "../include/sht31_wifi.h"


static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
        case WIFI_EVENT_STA_START:
            ESP_LOGI(WIFI_TAG, "Connecting to the Access Point.");
            esp_wifi_connect();
            break;
        
        case WIFI_EVENT_STA_DISCONNECTED:
            if (retry_counter < MAX_FAILURES)
            {
                ESP_LOGI(WIFI_TAG, "Trying to connect to the Access Point.");
                esp_wifi_connect();
                retry_counter++;
            }

            else
            {
                xEventGroupSetBits(wifi_event_group, WIFI_FAILURE);
            }
            break;
        
        default:
            ESP_LOGI(WIFI_TAG, "Something unexpected might just happened in the wifi event handler :O");
            ESP_LOGI(WIFI_TAG, "The event id was %d", (int)event_id);
            break;
        }
    }
}

static void ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == IP_EVENT)
    {
        switch (event_id)
        {
        case IP_EVENT_STA_GOT_IP:
            ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
            ESP_LOGI(WIFI_TAG, "STATION IP: " IPSTR, IP2STR(&event->ip_info.ip));
            retry_counter = 0;
            xEventGroupSetBits(wifi_event_group, WIFI_SUCCESS);
            break;
        
        default:
            ESP_LOGI(WIFI_TAG, "Something unexpected might just happened in the ip event handler :O");
            break;
        }
    }
}

esp_err_t wifi_start(void)
{
    int status = WIFI_FAILURE;
    
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_ESP_WIFI_SSID,
            .password = CONFIG_ESP_WIFI_PASSWORD,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(WIFI_TAG, "Station initialization completed.");

    EventBits_t bits = xEventGroupWaitBits(wifi_event_group,
                                        WIFI_SUCCESS | WIFI_FAILURE,
                                        pdFALSE,
                                        pdFALSE,
                                        portMAX_DELAY);

    if (bits & WIFI_SUCCESS)
    {
        ESP_LOGI(WIFI_TAG, "Succesfully connected to the Access Point.");
        status = WIFI_SUCCESS;
    }

    else if (bits & WIFI_FAILURE)
    {
        ESP_LOGI(WIFI_TAG, "Failed to connect to the Access Point.");
        status = WIFI_FAILURE;
    }

    else
    {
        ESP_LOGE(WIFI_TAG, "Uh Oh I just ran into a unexpected error in while starting the WIFI.");
        status = WIFI_FAILURE;
    }

    return status;
}

esp_err_t wifi_connect()
{
    int status;
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&config));

    wifi_event_group = xEventGroupCreate();

    esp_event_handler_instance_t wifi_handler_event_instance;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID, 
                                                        &wifi_event_handler, 
                                                        NULL, 
                                                        &wifi_handler_event_instance));

    esp_event_handler_instance_t ip_handler_event_instance;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP, 
                                                        &ip_event_handler, 
                                                        NULL, 
                                                        &ip_handler_event_instance));
    status = wifi_start();

    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_handler_event_instance));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, ip_handler_event_instance));
    vEventGroupDelete(wifi_event_group);

    return status;
}