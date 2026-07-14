idf_component_register(
    SRCS "led_driver.cpp"
    INCLUDE_DIRS "include"
    REQUIRES esp_common esp_system driver
)
