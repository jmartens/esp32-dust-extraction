# HTTP Server

The `http_server` component wraps ESP-IDF's HTTP server and accepts a
transport-independent request handler. The configured port defaults to 80.
Only GET and POST requests are forwarded; other methods receive a JSON 405
response. The handler supplies the response status and JSON body.

On the Linux host-test target the ESP-IDF server is unavailable. Request
dispatch remains testable there, while `start()` returns an explanatory error.
The firmware target uses ESP-IDF's `esp_http_server` component.

## Routing and startup

The http_routes component composes the handler passed to HttpServer:

| Method | Path        | Result                                                   |
| ------ | ----------- | -------------------------------------------------------- |
| GET    | `/`         | Static collector page, `Content-Type: text/html`         |
| other  | `/`         | 405 JSON error                                           |
| GET/POST | `/api/*`  | Forwarded unchanged to `collector_api` (status and body) |
| any    | other paths | 404 JSON error                                           |

Query strings are ignored for routing. Unsupported HTTP methods are rejected
by `HttpServer` with 405 before routing.

`app_main` wires the collector service, API and routes, then starts the
server. The port comes from `CONFIG_OPEN_EXTRACTION_HTTP_PORT`
(`idf.py menuconfig` > *Open Extraction* > *HTTP server port*, default 80,
range 1-65535). Until a hardware-backed collector unit exists, `app_main`
uses a state-machine-backed unit that confirms transitions immediately.
