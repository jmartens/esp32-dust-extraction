idf_component_register(
    SRCS "src/esp_relay_driver.cpp"
    INCLUDE_DIRS "include"
    REQUIRES esp_driver_gpio relay
)
