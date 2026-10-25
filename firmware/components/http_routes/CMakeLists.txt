idf_component_register(
    SRCS "src/http_routes.cpp"
    INCLUDE_DIRS "include"
    REQUIRES collector_api collector_page http_server
)
