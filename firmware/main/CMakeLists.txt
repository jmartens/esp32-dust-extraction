idf_component_register(
    SRCS "main.cpp"
    PRIV_REQUIRES
        collector_api
        collector_commands
        collector_service
        collector_state_machine
        collector_unit
        command_dispatcher
        event
        event_id_generator
        http_routes
        http_server
        esp_hw_support
        log
)
