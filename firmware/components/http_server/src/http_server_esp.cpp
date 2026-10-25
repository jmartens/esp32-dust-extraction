#include "open_extraction/http_server/http_server.hpp"

#include <string>

#include "esp_err.h"
#include "esp_http_server.h"

namespace open_extraction::http_server {

namespace {

const char *reason_phrase(int status) {
  switch (status) {
  case 200:
    return "OK";
  case 202:
    return "Accepted";
  case 400:
    return "Bad Request";
  case 404:
    return "Not Found";
  case 405:
    return "Method Not Allowed";
  case 409:
    return "Conflict";
  case 500:
    return "Internal Server Error";
  default:
    return "Response";
  }
}

esp_err_t dispatch_request(httpd_req_t *request) {
  auto *server = static_cast<HttpServer *>(request->user_ctx);
  Method method = Method::unsupported;
  if (request->method == HTTP_GET) {
    method = Method::get;
  } else if (request->method == HTTP_POST) {
    method = Method::post;
  }

  const Response response = server->handle({method, request->uri});
  const std::string status =
      std::to_string(response.status) + " " + reason_phrase(response.status);
  if (httpd_resp_set_type(request, "application/json") != ESP_OK ||
      httpd_resp_set_status(request, status.c_str()) != ESP_OK) {
    return ESP_FAIL;
  }
  return httpd_resp_send(request, response.body.c_str(),
                         static_cast<ssize_t>(response.body.size()));
}

} // namespace

struct Platform {
  static std::string start(HttpServer &server) {
    if (server.config_.port == 0) {
      return "HTTP server port must be between 1 and 65535";
    }
    if (server.native_handle_ != nullptr) {
      return "HTTP server is already running";
    }

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = server.config_.port;
    config.uri_match_fn = httpd_uri_match_wildcard;

    httpd_handle_t handle = nullptr;
    const esp_err_t start_result = httpd_start(&handle, &config);
    if (start_result != ESP_OK) {
      return std::string("httpd_start failed: ") +
             esp_err_to_name(start_result);
    }

    httpd_uri_t route = {
        .uri = "/*",
        .method = static_cast<httpd_method_t>(HTTP_ANY),
        .handler = dispatch_request,
        .user_ctx = &server,
    };
    const esp_err_t register_result =
        httpd_register_uri_handler(handle, &route);
    if (register_result != ESP_OK) {
      (void)httpd_stop(handle);
      return std::string("httpd_register_uri_handler failed: ") +
             esp_err_to_name(register_result);
    }

    server.native_handle_ = handle;
    return {};
  }

  static std::string stop(HttpServer &server) {
    if (server.native_handle_ == nullptr) {
      return {};
    }
    const esp_err_t result =
        httpd_stop(static_cast<httpd_handle_t>(server.native_handle_));
    if (result != ESP_OK) {
      return std::string("httpd_stop failed: ") + esp_err_to_name(result);
    }
    server.native_handle_ = nullptr;
    return {};
  }
};

std::string HttpServer::start() { return Platform::start(*this); }

std::string HttpServer::stop() { return Platform::stop(*this); }

} // namespace open_extraction::http_server
