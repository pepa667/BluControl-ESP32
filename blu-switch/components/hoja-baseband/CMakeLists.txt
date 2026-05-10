set(BB_MAIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/HOJA-ESP32-Baseband/main)

idf_component_register(
    SRCS
        "${BB_MAIN_DIR}/core_bt_switch.c"
        "${BB_MAIN_DIR}/core_bt_sinput.c"
        "${BB_MAIN_DIR}/util_bt_hid.c"
        "${BB_MAIN_DIR}/switch_commands.c"
        "${BB_MAIN_DIR}/switch_spi.c"
        "${BB_MAIN_DIR}/switch_analog.c"
        "${BB_MAIN_DIR}/switch_haptics.c"
        "${BB_MAIN_DIR}/imu_tool.c"
        "${BB_MAIN_DIR}/interval.c"
        "${BB_MAIN_DIR}/esp_hid_gap.c"
        "baseband_bridge.c"
    INCLUDE_DIRS
        "include"
    PRIV_INCLUDE_DIRS
        "${BB_MAIN_DIR}"
        "${BB_MAIN_DIR}/include"
        "${BB_MAIN_DIR}/include/switch"
    REQUIRES
        bt
        nvs_flash
        driver
        esp_hid
        esp_timer
        esp_adc
        freertos
        log
        esp_system
        esp_common
        soc
        hal
        heap
        newlib
)

# Baseband uses C11 generics and has benign maybe-uninitialized in macros; suppress those.
target_compile_options(__idf_hoja-baseband PRIVATE
    -Wno-maybe-uninitialized
    -Wno-unused-variable
)
