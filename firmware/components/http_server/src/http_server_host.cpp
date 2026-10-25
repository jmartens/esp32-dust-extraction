#include "open_extraction/http_server/http_server.hpp"

namespace open_extraction::http_server {

std::string HttpServer::start() {
  return "ESP-IDF HTTP server is unavailable on the Linux host target";
}

std::string HttpServer::stop() {
  if (native_handle_ == nullptr) {
    return {};
  }
  return "ESP-IDF HTTP server is unavailable on the Linux host target";
}

} // namespace open_extraction::http_server
