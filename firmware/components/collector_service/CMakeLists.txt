idf_component_register(
    SRCS "src/collector_service.cpp"
    INCLUDE_DIRS "include"
    REQUIRES
        collector_api
        collector_commands
        collector_events
        collector_unit
        command
        command_dispatcher
        event
        event_dispatcher
)
