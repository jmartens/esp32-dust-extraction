idf_component_register(
    SRCS "src/relay_backed_collector_unit.cpp"
    INCLUDE_DIRS "include"
    REQUIRES collector_state_machine collector_unit relay
)
