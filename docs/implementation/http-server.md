# HTTP Server

The `http_server` component wraps ESP-IDF's HTTP server and accepts a
transport-independent request handler. The configured port defaults to 80.
Only GET and POST requests are forwarded; other methods receive a JSON 405
response. The handler supplies the response status and JSON body.

On the Linux host-test target the ESP-IDF server is unavailable. Request
dispatch remains testable there, while `start()` returns an explanatory error.
The firmware target uses ESP-IDF's `esp_http_server` component.
