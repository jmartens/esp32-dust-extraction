# Collector HTTP API (v1)

Minimal JSON API for manual collector control. The handler lives in
`firmware/components/collector_api` and is transport-agnostic
([ADR-0011](../adr/0011-transport-agnostic-business-logic.md)); the HTTP server
(#17) only routes requests to it.

Commands are requests, not state changes
([ADR-0009](../adr/0009-separate-event-bus-and-command-bus.md)). A start or
stop response therefore reports the state observed at response time, which may
still be the previous state until the resulting events are processed. Clients
should poll `GET /api/v1/collector`.

## Endpoints

| Method | Path                        | Success | Description                    |
| ------ | --------------------------- | ------- | ------------------------------ |
| GET    | `/api/v1/collector`         | 200     | Current collector state        |
| POST   | `/api/v1/collector/start`   | 202     | Request that the collector start |
| POST   | `/api/v1/collector/stop`    | 202     | Request that the collector stop  |

## Collector resource

```json
{ "state": "stopped", "device_uid": "collector-01" }
```

`state` is one of `stopped`, `starting`, `running`, `stopping`, `faulted`.
For the relay-backed Collector Unit, these states describe the commanded
relay/output state. `running` means the relay has been commanded on; it does
not confirm that the motor is physically running because no motor-feedback
sensor is present.

## Relay-backed Collector Unit

The production Collector Unit composes the collector state machine with the
relay driver. At startup it commands the relay off before accepting commands.
A permitted start commands the relay on and reports `running`; a permitted
stop commands it off and reports `stopped`. Requests rejected by the state
machine do not change the relay output.

Configure the relay under **Open Extraction** in `idf.py menuconfig`:

| Setting | Default | Description |
| ------- | ------- | ----------- |
| Collector relay GPIO pin | GPIO 26 | Output pin connected to the relay |
| Collector relay polarity | Active high | Select active high or active low |

Choose an output-capable pin that is not reserved by the board or flash
interface. The ESP relay driver presets the GPIO's off level before enabling
output mode, so active-low relays are not briefly energized during
initialization.

## Errors

Errors use the body `{ "error": "<message>" }`.

| Status | Meaning                                                   |
| ------ | --------------------------------------------------------- |
| 404    | Unknown path                                              |
| 405    | Method not supported for the path                         |
| 409    | Request not permitted in the current collector state      |

## Collector service

`firmware/components/collector_service` provides `CollectorService`, the
concrete `ICollectorControl` the API depends on. It:

- dispatches `StartCollector` / `StopCollector` (targeted at the collector
  Device UID) through the `CommandDispatcher`; `request_*` returns `true` only
  when the command was delivered, otherwise the API answers 409;
- rejects a request without dispatching when the observed state does not allow
  it (start only from `stopped`, stop only from `running`);
- reports `state()` from the `ICollectorUnit` and `device_uid()` from its
  configured Device UID;
- never changes state itself. Transitions happen only when the command
  recipient handles the command and the resulting events are processed
  ([ADR-0009](../adr/0009-separate-event-bus-and-command-bus.md)).
- publishes an immutable `CollectorFault` Domain Event through the
  `EventDispatcher` when `report_fault(reason)` is called. The event includes
  its reason in JSON serialization; state handling remains with event
  consumers.

Routing HTTP to `CollectorApi` and the startup wiring are described in
[HTTP Server](http-server.md).

## HTML control page

The page is a static file, `firmware/components/collector_page/web/index.html`
(no external assets). CMake compiles it into the firmware by generating a
source file at configure time, so it works on every target, including the
Linux host tests. Edit the HTML file directly; do not edit generated sources.

It shows the collector state and Device UID and offers Start and Stop actions.
It uses only the endpoints above and polls `GET /api/v1/collector` every two
seconds to show the confirmed state. The page is served at `GET /` with
`Response::content_type = "text/html"` by the `http_routes` component.

The page is intentionally framework-free for the MVP. A richer UI is expected
later and will need a frontend framework and build step, which is out of scope
here.
