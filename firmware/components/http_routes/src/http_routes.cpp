#include "open_extraction/http_routes/http_routes.hpp"

#include <string>
#include <string_view>

#include "open_extraction/collector_page/collector_page.hpp"

namespace open_extraction::http_routes {

namespace {

constexpr std::string_view kApiPrefix{"/api/"};

http_server::Response error(int status, std::string_view message) {
  return {status, "{\"error\":\"" + std::string(message) + "\"}"};
}

std::string_view strip_query(std::string_view uri) {
  return uri.substr(0, uri.find_first_of("?#"));
}

} // namespace

http_server::RequestHandler
make_request_handler(const collector_api::CollectorApi &api) {
  return [&api](const http_server::Request &request) -> http_server::Response {
    const std::string_view path = strip_query(request.path);

    if (path == "/") {
      if (request.method != http_server::Method::get) {
        return error(405, "method not allowed");
      }
      return {200, std::string(collector_page::html()), "text/html"};
    }

    if (path.starts_with(kApiPrefix)) {
      collector_api::Method method;
      switch (request.method) {
      case http_server::Method::get:
        method = collector_api::Method::get;
        break;
      case http_server::Method::post:
        method = collector_api::Method::post;
        break;
      default:
        return error(405, "method not allowed");
      }
      collector_api::Response response = api.handle(method, path);
      return {response.status, std::move(response.body)};
    }

    return error(404, "not found");
  };
}

} // namespace open_extraction::http_routes
