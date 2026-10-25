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

## Errors

Errors use the body `{ "error": "<message>" }`.

| Status | Meaning                                                   |
| ------ | --------------------------------------------------------- |
| 404    | Unknown path                                              |
| 405    | Method not supported for the path                         |
| 409    | Request not permitted in the current collector state      |
