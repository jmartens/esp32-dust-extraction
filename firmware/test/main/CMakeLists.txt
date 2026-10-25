set(OPEN_EXTRACTION_COMPONENTS_DIR
    "${CMAKE_CURRENT_LIST_DIR}/../../components"
)
set(COLLECTOR_EVENTS_TEST_DIR
    "${OPEN_EXTRACTION_COMPONENTS_DIR}/collector_events/test"
)
set(COLLECTOR_UNIT_TEST_DIR
    "${OPEN_EXTRACTION_COMPONENTS_DIR}/collector_unit/test"
)
set(COMMAND_TEST_DIR "${OPEN_EXTRACTION_COMPONENTS_DIR}/command/test")
set(COMMAND_DISPATCHER_TEST_DIR
    "${OPEN_EXTRACTION_COMPONENTS_DIR}/command_dispatcher/test"
)
set(EVENT_TEST_DIR "${OPEN_EXTRACTION_COMPONENTS_DIR}/event/test")
set(EVENT_DISPATCHER_TEST_DIR
    "${OPEN_EXTRACTION_COMPONENTS_DIR}/event_dispatcher/test"
)
set(EVENT_ID_TEST_DIR
    "${OPEN_EXTRACTION_COMPONENTS_DIR}/event_id_generator/test"
)
set(EVENT_JSON_TEST_DIR "${OPEN_EXTRACTION_COMPONENTS_DIR}/event_json/test")
set(RELAY_TEST_DIR "${OPEN_EXTRACTION_COMPONENTS_DIR}/relay/test")

idf_component_register(
    SRCS
        "test_main.cpp"
        "${COLLECTOR_EVENTS_TEST_DIR}/test_collector_events.cpp"
        "${COLLECTOR_UNIT_TEST_DIR}/test_collector_unit.cpp"
        "${COMMAND_TEST_DIR}/test_command.cpp"
        "${COMMAND_DISPATCHER_TEST_DIR}/test_command_dispatcher.cpp"
        "${EVENT_TEST_DIR}/test_event.cpp"
        "${EVENT_DISPATCHER_TEST_DIR}/test_event_dispatcher.cpp"
        "${EVENT_ID_TEST_DIR}/test_esp_event_id_generator.cpp"
        "${EVENT_JSON_TEST_DIR}/test_event_json_codec.cpp"
        "${RELAY_TEST_DIR}/test_relay_driver.cpp"
    PRIV_REQUIRES
        unity
        collector_events
        collector_unit
        command
        command_dispatcher
        event
        event_dispatcher
        event_id_generator
        event_json
        relay
    WHOLE_ARCHIVE
)
