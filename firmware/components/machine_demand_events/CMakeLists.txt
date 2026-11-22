idf_component_register(
    SRCS "src/machine_demand_started_emitter.cpp"
    INCLUDE_DIRS "include"
    REQUIRES event event_dispatcher machine_demand_threshold
)
