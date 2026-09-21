idf_component_register(
    SRCS "test_esp_relay_driver.cpp"
    PRIV_REQUIRES unity esp_driver_gpio esp_relay_driver
)
