idf_component_register(
    SRCS "src/esp_adc_driver.cpp"
    INCLUDE_DIRS "include"
    REQUIRES adc esp_adc
    PRIV_REQUIRES esp_timer
)
