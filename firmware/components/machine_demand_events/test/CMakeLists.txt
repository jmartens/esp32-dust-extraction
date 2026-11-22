idf_component_register(
    SRCS
        "test_machine_demand_started.cpp"
        "test_machine_demand_events.cpp"
    PRIV_REQUIRES unity event_json machine_demand_events
)
