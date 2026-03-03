idf_component_register(SRCS "firmware.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_http_client esp_wifi nvs_flash driver)
