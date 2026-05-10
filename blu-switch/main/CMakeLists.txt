idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES
        hoja-baseband
        blu-components
        BluControl-SDK-ESP32
        driver
        esp_timer
        freertos
        log
        nvs_flash
)
