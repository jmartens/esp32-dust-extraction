set(http_server_sources "src/http_server.cpp")
set(http_server_component_args)

if(IDF_TARGET STREQUAL "linux")
    list(APPEND http_server_sources "src/http_server_host.cpp")
else()
    list(APPEND http_server_sources "src/http_server_esp.cpp")
    list(APPEND http_server_component_args REQUIRES esp_http_server)
endif()

idf_component_register(
    SRCS ${http_server_sources}
    INCLUDE_DIRS "include"
    ${http_server_component_args}
)
