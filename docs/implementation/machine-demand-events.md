# Machine demand started events

`MachineDemandStartedEmitter` owns a threshold evaluator for one Machine Unit.
Call `evaluate` with each calibrated RMS reading and a monotonic timestamp. A
debounced inactive-to-active transition publishes `machine_demand.started` to
the event dispatcher with a newly generated Event ID, wall-clock timestamp,
Device UID and Unit Identifier (`unit_id` in JSON). Invalid readings and
unchanged or stopped transitions do not publish a started event. The returned
transition remains available to the caller.

The emitter is a hardware-independent building block; the firmware does not yet
connect it to a Machine Unit's sensor sampling loop. Physical sensor and
end-to-end event delivery therefore require hardware integration and validation.
