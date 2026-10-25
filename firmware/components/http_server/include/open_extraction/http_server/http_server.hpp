#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <string_view>

namespace open_extraction::http_server {

enum class Method { get, post, unsupported };

struct Request {
  Method method;
  std::string_view path;
};

struct Response {
  int status;
  std::string body;
};

struct Config {
  std::uint16_t port = 80;
};

using RequestHandler = std::function<Response(const Request &)>;

class HttpServer {
public:
  explicit HttpServer(Config config, RequestHandler handler);
  ~HttpServer();

  HttpServer(const HttpServer &) = delete;
  HttpServer &operator=(const HttpServer &) = delete;
  HttpServer(HttpServer &&) = delete;
  HttpServer &operator=(HttpServer &&) = delete;

  [[nodiscard]] Response handle(const Request &request) const;

  // Empty means success; otherwise the returned string explains the failure.
  [[nodiscard]] std::string start();
  [[nodiscard]] std::string stop();

  [[nodiscard]] std::uint16_t port() const noexcept { return config_.port; }

private:
  friend struct Platform;

  Config config_;
  RequestHandler handler_;
  void *native_handle_ = nullptr;
};

} // namespace open_extraction::http_server
