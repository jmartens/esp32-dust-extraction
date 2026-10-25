#pragma once

#include "open_extraction/collector_api/collector_api.hpp"
#include "open_extraction/http_server/http_server.hpp"

namespace open_extraction::http_routes {

// Composes the HTTP routes: GET / serves the static collector page and
// /api/* is forwarded to the collector API. The API must outlive the handler.
// Query strings are ignored for routing. Unknown paths receive a JSON 404.
[[nodiscard]] http_server::RequestHandler
make_request_handler(const collector_api::CollectorApi &api);

} // namespace open_extraction::http_routes
