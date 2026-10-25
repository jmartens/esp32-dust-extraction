idf_component_register(
    SRCS "test_collector_service.cpp"
    PRIV_REQUIRES
        unity
        collector_commands
        collector_service
        collector_unit
        command
        command_dispatcher
        event
)
