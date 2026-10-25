#include "open_extraction/http_server/http_server.hpp"

#include <utility>

namespace open_extraction::http_server {

HttpServer::HttpServer(Config config, RequestHandler handler)
    : config_(config), handler_(std::move(handler)) {}

HttpServer::~HttpServer() {
  if (native_handle_ != nullptr) {
    (void)stop();
  }
}

Response HttpServer::handle(const Request &request) const {
  if (request.method != Method::get && request.method != Method::post) {
    return {405, "{\"error\":\"method not allowed\"}"};
  }
  if (!handler_) {
    return {500, "{\"error\":\"request handler unavailable\"}"};
  }
  return handler_(request);
}

} // namespace open_extraction::http_server
