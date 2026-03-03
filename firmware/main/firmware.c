#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/i2c.h"
#include "esp_http_client.h"

#include "lwip/err.h"
#include "lwip/sys.h"

// =================== CONFIG WIFI ===================
#define ESP_WIFI_SSID CONFIG_ESP_WIFI_SSID
#define ESP_WIFI_PASS CONFIG_ESP_WIFI_PASSWORD
#define ESP_MAXIMUM_RETRY 10

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static EventGroupHandle_t s_wifi_event_group;
static const char *TAG = "wifi_station";
static int s_retry_num = 0;

// =================== CONFIG BH1750 ===================
#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000
#define I2C_MASTER_TX_BUF_DISABLE 0
#define I2C_MASTER_RX_BUF_DISABLE 0

#define BH1750_ADDR 0x23
#define BH1750_POWER_ON 0x01
#define BH1750_RESET 0x07
#define BH1750_CONT_H_RES_MODE 0x10

// =================== CONFIG URL ===================
#define WEB_SERVER_URL "http://your_ip:3000/api/data"

// =================== WIFI HANDLER ===================
static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < ESP_MAXIMUM_RETRY)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG, "connect to the AP fail");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = ESP_WIFI_SSID,
            .password = ESP_WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE, pdFALSE, portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s", ESP_WIFI_SSID, ESP_WIFI_PASS);
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s", ESP_WIFI_SSID, ESP_WIFI_PASS);
    }
    else
    {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
}

// =================== BH1750 FUNCTIONS ===================
esp_err_t bh1750_init()
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    esp_err_t err = i2c_param_config(I2C_MASTER_NUM, &conf);
    if (err != ESP_OK)
    {
        ESP_LOGE("BH1750", "i2c_param_config failed: %s", esp_err_to_name(err));
        return err;
    }

    err = i2c_driver_install(I2C_MASTER_NUM, conf.mode,
                             I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
    if (err != ESP_OK)
    {
        ESP_LOGE("BH1750", "i2c_driver_install failed: %s", esp_err_to_name(err));
        return err;
    }

    uint8_t cmd = BH1750_POWER_ON;
    err = i2c_master_write_to_device(I2C_MASTER_NUM, BH1750_ADDR, &cmd, 1, 1000 / portTICK_PERIOD_MS);
    if (err != ESP_OK)
    {
        ESP_LOGE("BH1750", "POWER_ON failed: %s", esp_err_to_name(err));
        return err;
    }

    cmd = BH1750_CONT_H_RES_MODE;
    err = i2c_master_write_to_device(I2C_MASTER_NUM, BH1750_ADDR, &cmd, 1, 1000 / portTICK_PERIOD_MS);
    if (err != ESP_OK)
    {
        ESP_LOGE("BH1750", "SET_MODE failed: %s", esp_err_to_name(err));
    }

    return err;
}

float bh1750_read_light(void)
{
    const char *TAG_BH = "BH1750";
    uint8_t data[2];
    
    esp_err_t ret = i2c_master_read_from_device(I2C_MASTER_NUM, BH1750_ADDR, data, 2, 1000 / portTICK_PERIOD_MS);
    
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG_BH, "Failed to read from BH1750: %s", esp_err_to_name(ret));
        return -1.0f;
    }

    uint16_t raw = (data[0] << 8) | data[1];
    
    float lux = (float)raw / 1.2f;
    
    ESP_LOGD(TAG_BH, "BH1750 raw data: %d, lux: %.2f", raw, lux);
    
    return lux;
}

// =================== SEND DATA TO SERVER ===================
void send_data_to_server(float lux)
{
    esp_http_client_config_t config = {
        .url = WEB_SERVER_URL,
        .method = HTTP_METHOD_POST,
        .timeout_ms = 10000
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    char post_data[128];
    sprintf(post_data, "{\"device_id\": \"bh1750 \", \"lux\": %.2f}", lux);

    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK)
    {
        ESP_LOGI("HTTP", "Data has been successfully sent! Code: %d", esp_http_client_get_status_code(client));
    }
    else
    {
        ESP_LOGE("HTTP", "Error sending data: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}

// =================== MAIN ===================
void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();

    ESP_LOGI("BH1750", "Initializing the light sensor....");
    if (bh1750_init() != ESP_OK)
    {
        ESP_LOGE("BH1750", "Unable to initialize sensor!");
        vTaskDelete(NULL);
    }

    while (1)
    {
        float lux = bh1750_read_light();
        if (lux >= 0)
        {
            ESP_LOGI("BH1750", "Light intensity: %.2f lx", lux);
            send_data_to_server(lux);
        }
        else
        {
            ESP_LOGW("BH1750", "Sensor disconnected! Trying to reconnect...");

            int retry_count = 0;
            while (retry_count < ESP_MAXIMUM_RETRY)
            {
                if (bh1750_init() == ESP_OK)
                {
                    ESP_LOGI("BH1750", "Sensor successfully reconnected!");
                    break;
                }
                else
                {
                    ESP_LOGW("BH1750", "Trying again (%d/10)...", retry_count + 1);
                    vTaskDelay(pdMS_TO_TICKS(1000));
                }
                retry_count++;
            }

            if (retry_count == 10)
            {
                ESP_LOGE("BH1750", "Unable to reconnect the sensor after 10 attempts!");
            }
        }

        vTaskDelay(pdMS_TO_TICKS(2000)); 
    }
}