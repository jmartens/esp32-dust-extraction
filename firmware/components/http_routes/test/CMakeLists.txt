idf_component_register(
    SRCS "test_http_routes.cpp"
    PRIV_REQUIRES unity collector_api collector_unit http_routes http_server
)
