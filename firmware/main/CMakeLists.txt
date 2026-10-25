idf_component_register(
    SRCS "main.cpp"
    PRIV_REQUIRES
        collector_api
        collector_commands
        collector_service
        relay_backed_collector_unit
        esp_relay_driver
        command_dispatcher
        event
        event_id_generator
        http_routes
        http_server
        esp_hw_support
        log
)
