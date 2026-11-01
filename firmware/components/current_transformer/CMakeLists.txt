idf_component_register(
    SRCS "src/current_transformer.cpp"
    INCLUDE_DIRS "include"
    REQUIRES adc current_sensor
)
