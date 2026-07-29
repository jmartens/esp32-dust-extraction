# scripts/bootstrap-github.sh

#!/usr/bin/env bash

set -euo pipefail

REPO=$(gh repo view --json owner,name \
    --jq '.owner.login + "/" + .name')

echo "Using repository: $REPO"

create_label() {
    echo "Create label: $1"
    gh label create "$1" \
        --color "$2" \
        --force >/dev/null
}

create_milestone() {
    echo "Create milestone: $1"
    gh api repos/$REPO/milestones \
        --method POST \
        -f title="$1" >/dev/null 2>&1 || true
}

issue_exists() {
    gh issue list \
        --limit 100 \
        --search "$1 in:title" \
        --json title \
        --jq '.[].title' | grep -Fxq "$1"
}

create_issue() {
    local title="$1"
    local label="$2"

    if issue_exists "$title"; then
        echo "SKIP: $title"

        # Consume heredoc if present.
        cat >/dev/null || true

        return
    fi
	
    echo "Create issue: $title ($label)"

    gh issue create \
        --title "$title" \
        --label "$label" \
        --body-file -
}

create_epic() {
    local title="$1"

    if issue_exists "$title"; then
        echo "SKIP: $title"
        return
    fi

    gh issue create \
        --title "$title" \
        --label "epic" \
        --body-file -
}

###############################################################################

# Labels

###############################################################################

create_label "epic" "5319E7"
create_label "feature" "0E8A16"
create_label "bug" "D73A4A"
create_label "documentation" "0075CA"
create_label "adr" "1D76DB"

###############################################################################

# Milestones

###############################################################################

create_milestone "v0.0.1 - Repository Bootstrap"
create_milestone "v0.1.0 - First Dust"
create_milestone "v0.2.0 - Automatic Extraction"
create_milestone "v0.3.0 - Workshop MVP"

###############################################################################
# Milestone: v0.1.0 - First Dust
#
# Acceptance Criteria:
#
# - Collector can be started remotely.
# - Collector can be stopped remotely.
# - Collector state is reported correctly.
# - ESP-NOW transport is functional.
# - System operates successfully in the workshop.
###############################################################################

create_epic "[EPIC] Event System" <<'EOF'
## Goal

Provide a deterministic event-driven foundation for OpenExtraction.

## Acceptance Criteria

- Events are immutable.
- Events may have multiple subscribers.
- Event processing is deterministic.
- Event processing is unit testable.

## Child Issues

- Implement Event base class
- Implement Event Dispatcher
- Implement Command base class
- Implement Command Dispatcher

## References

- 03-event-model.md
- 04-state-machines.md
EOF

create_epic "[EPIC] Collector Service" <<'EOF'
## Goal

Provide centralized control of the dust collector.

## Acceptance Criteria

- Collector can be started.
- Collector can be stopped.
- Collector state is tracked.
- Collector faults are reported.
EOF

create_epic "[EPIC] Collector Unit" <<'EOF'
## Goal

Allow a physical device to control the dust collector.

## Acceptance Criteria

- Relay output can be controlled.
- Collector state is reported.
- Collector commands are executed reliably.
EOF

create_epic "[EPIC] HTTP Interface" <<'EOF'
## Goal

Provide a minimal HTTP interface.

## Acceptance Criteria

- Collector can be started via HTTP.
- Collector can be stopped via HTTP.
- Collector state can be queried.
EOF

create_issue "Implement Event base class" "feature" <<'EOF'
## Description

Implement the abstract base Event type used by all OpenExtraction
business events.

The Event class is the foundation of the event-driven architecture and
must remain transport, hardware and platform independent.

## Acceptance Criteria

- Event is immutable after construction.
- Event exposes a unique EventType identifier.
- Event contains a UTC timestamp.
- Event contains an originating Device UID.
- Event contains a globally unique Event ID.
- Event is serializable.
- Event is copyable and movable.
- Event has no ESP-IDF dependencies.
- Native unit tests are implemented.
- CI passes.

## Technical Notes

Suggested interface:

- event_id()
- event_type()
- timestamp()
- originating_device()

Use:

- std::chrono
- std::string_view where applicable
- constexpr EventType values

## Out of Scope

- Event transport.
- Event persistence.
- Event subscriptions.

## References

- Parent Epic: [EPIC] Event System
- docs/specification/03-event-model.md
- docs/specification/05-service-model.md
EOF

create_issue "Implement Event Dispatcher" "feature" <<'EOF'
## Description

Implement the central Event Dispatcher responsible for deterministic
delivery of Events to subscribers.

This component forms the backbone of Controller communication.

## Acceptance Criteria

- Supports multiple subscribers per EventType.
- Preserves event ordering.
- Supports subscribe().
- Supports unsubscribe().
- Supports publish().
- Supports zero subscribers.
- Supports multiple Event types.
- Unknown Event types are handled gracefully.
- Native unit tests included.
- CI passes.

## Technical Notes

Suggested interface:

- subscribe()
- unsubscribe()
- publish()

Dispatcher SHALL:

- contain no business logic.
- contain no transport logic.
- remain single-threaded for MVP.

## Out of Scope

- Thread safety.
- Distributed event dispatching.
- Event persistence.

## References

- Parent Epic: [EPIC] Event System
- docs/specification/03-event-model.md
EOF

create_issue "Implement Command base class" "feature" <<'EOF'
## Description

Implement the abstract base Command type.

Commands represent requests for behavior and SHALL always target a
single recipient.

## Acceptance Criteria

- Command is immutable.
- Command contains a CommandType.
- Command contains a target Device UID.
- Command contains a globally unique Command ID.
- Command contains a UTC timestamp.
- Command is serializable.
- Command is copyable and movable.
- No ESP-IDF dependencies.
- Native unit tests included.
- CI passes.

## Technical Notes

Suggested interface:

- command_id()
- command_type()
- target()
- timestamp()

## Out of Scope

- Command execution.
- Transport.
- Retries.
- Acknowledgements.

## References

- Parent Epic: [EPIC] Event System
- docs/specification/03-event-model.md
EOF

create_issue "Implement Command Dispatcher" "feature" <<'EOF'
## Description

Implement the Command Dispatcher responsible for routing Commands to
their intended recipients.

## Acceptance Criteria

- Supports single recipient Commands.
- Supports registration of recipients.
- Supports deregistration of recipients.
- Supports unknown recipients.
- Reports delivery failures.
- Unit tests included.
- CI passes.

## Technical Notes

Suggested interface:

- register_recipient()
- unregister_recipient()
- dispatch()

For MVP:

- In-memory only.
- Single Controller process.
- No network transport.

## Out of Scope

- Retries.
- Persistence.
- Network delivery.
- Authentication.

## References

- Parent Epic: [EPIC] Event System
- docs/specification/03-event-model.md
EOF

create_issue "Implement Collector State Machine" "feature" <<'EOF'
## Description

Implement the Collector state machine responsible for managing the
lifecycle of the dust collector.

## Acceptance Criteria

- Supports Stopped state.
- Supports Starting state.
- Supports Running state.
- Supports Stopping state.
- Supports Fault state.
- Invalid transitions are rejected.
- State transitions emit Events.
- Unit tests included.
- CI passes.

## Technical Notes

Suggested transitions:

Stopped -> Starting
Starting -> Running
Running -> Stopping
Stopping -> Stopped
* -> Fault

## References

- Parent Epic: [EPIC] Collector Service
- docs/specification/04-state-machines.md
EOF

create_issue "Implement StartCollector command" "feature" <<'EOF'
## Description

Implement the StartCollector Command.

## Acceptance Criteria

- Contains target Device UID.
- Contains timestamp.
- Is serializable.
- Is immutable.
- Unit tests included.
- CI passes.

## Technical Notes

This command SHALL NOT directly manipulate hardware.

## References

- Parent Epic: [EPIC] Collector Service
- docs/specification/03-event-model.md
EOF

create_issue "Implement StopCollector command" "feature" <<'EOF'
## Description

Implement the StopCollector Command.

## Acceptance Criteria

- Contains target Device UID.
- Contains timestamp.
- Is serializable.
- Is immutable.
- Unit tests included.
- CI passes.

## References

- Parent Epic: [EPIC] Collector Service
- docs/specification/03-event-model.md
EOF

create_issue "Implement CollectorStarted event" "feature" <<'EOF'
## Description

Implement the CollectorStarted Event emitted after successful startup.

## Acceptance Criteria

- Contains originating Device UID.
- Contains timestamp.
- Contains Event ID.
- Is immutable.
- Unit tests included.
- CI passes.

## References

- Parent Epic: [EPIC] Collector Service
EOF

create_issue "Implement CollectorStopped event" "feature" <<'EOF'
## Description

Implement the CollectorStopped Event emitted after successful shutdown.

## Acceptance Criteria

- Contains originating Device UID.
- Contains timestamp.
- Contains Event ID.
- Is immutable.
- Unit tests included.
- CI passes.

## References

- Parent Epic: [EPIC] Collector Service
EOF

create_issue "Implement Collector Unit interface" "feature" <<'EOF'
## Description

Implement the ICollectorUnit abstraction.

The Collector Unit represents a Unit hosted by a physical Device.

## Acceptance Criteria

- Supports start().
- Supports stop().
- Supports state().
- Supports diagnostics().
- Supports capability reporting.
- Unit tests included.
- CI passes.

## Technical Notes

Suggested interface:

- start()
- stop()
- state()
- diagnostics()

## References

- Parent Epic: [EPIC] Collector Unit
- ADR-0014
- docs/specification/05-service-model.md
EOF

create_issue "Implement Relay driver abstraction" "feature" <<'EOF'
## Description

Implement a hardware abstraction for relay outputs.

## Acceptance Criteria

- Supports ON operation.
- Supports OFF operation.
- Supports state queries.
- Hardware independent.
- Unit tests included.
- CI passes.

## Technical Notes

Business logic SHALL NOT manipulate GPIO directly.

## References

- Parent Epic: [EPIC] Collector Unit
- docs/specification/09-hardware-abstraction.md
EOF

create_issue "Implement ESP32 relay driver" "feature" <<'EOF'
## Description

Implement an ESP32-specific relay driver.

## Acceptance Criteria

- Supports configurable GPIO.
- Supports active HIGH relays.
- Supports active LOW relays.
- Hardware validated.
- Documentation updated.
- CI passes.

## Technical Notes

Use ESP-IDF GPIO APIs exclusively.

## References

- Parent Epic: [EPIC] Collector Unit
EOF

create_issue "Implement minimal HTTP server" "feature" <<'EOF'
## Description

Implement the minimal HTTP server for the Controller.

## Acceptance Criteria

- HTTP server starts successfully.
- Port is configurable.
- Supports GET requests.
- Supports POST requests.
- Unit tests included where possible.
- CI passes.

## Technical Notes

This is an MVP implementation only.

## References

- Parent Epic: [EPIC] HTTP Interface
EOF

create_issue "Implement collector API" "feature" <<'EOF'
## Description

Implement a minimal HTTP API for collector control.

## Acceptance Criteria

- Supports GET /api/v1/collector.
- Supports POST /api/v1/collector/start.
- Supports POST /api/v1/collector/stop.
- Returns JSON responses.
- API documented.
- CI passes.

## References

- Parent Epic: [EPIC] HTTP Interface
EOF

create_issue "Implement minimal HTML page" "feature" <<'EOF'
## Description

Implement a minimal web page for manual collector control.

## Acceptance Criteria

- Displays collector state.
- Displays collector Device UID.
- Supports Start action.
- Supports Stop action.
- Supports mobile browsers.
- Tested on desktop browsers.
- CI passes where applicable.

## Technical Notes

The page is intentionally minimal and intended for MVP use in the
workshop.

## References

- Parent Epic: [EPIC] HTTP Interface
- Milestone: v0.1.0 - First Dust
EOF

###############################################################################
# Milestone: v0.2.0 - Automatic Extraction
#
# Acceptance Criteria:
#
# - AF22 starts automatically.
# - AF22 stops automatically.
# - Supports at least one machine.
# - Runs continuously for 8 hours.
# - No missed starts observed.
# - No false positives observed.
# - Successfully used in the workshop.
###############################################################################

create_epic "[EPIC] Machine Demand Detection" <<'EOF'
## Goal

Provide a hardware-independent mechanism for detecting when machines
require dust extraction.

Machine Demand Detection is responsible for converting physical
measurements (current transformers, Hall effect sensors, power meters,
etc.) into deterministic MachineDemandStarted and
MachineDemandStopped Events.

## Acceptance Criteria

- Supports arbitrary Current Sensor implementations.
- Supports configurable machine profiles.
- Supports demand thresholds.
- Supports demand debounce.
- Supports calibration.
- Supports multiple simultaneous machines.
- Supports workshop validation.
- Produces deterministic MachineDemand Events.
- Is fully unit testable.
- Has no business logic outside the Controller.

## Child Issues

- Implement IMachineDemandSource interface
- Implement ESP32 ADC abstraction
- Implement Current Transformer abstraction
- Implement RMS current calculation
- Implement demand threshold evaluation
- Implement demand debounce support
- Implement current transformer calibration
- Create current transformer test fixture
- Implement MachineDemandStarted event
- Implement MachineDemandStopped event

## Out of Scope

- Automatic collector control.
- Gate control.
- Routing decisions.

## Definition of Done

The system reliably detects machine activity in a real workshop and
emits the appropriate MachineDemand Events.

## References

- ADR-0014
- docs/specification/03-event-model.md
- docs/specification/05-service-model.md
EOF

create_epic "[EPIC] Collector Automation" <<'EOF'
## Goal

Automatically control the dust collector based on active machine demand.

Collector Automation consumes MachineDemand Events and determines when
the collector should start or stop.

## Acceptance Criteria

- Supports automatic collector start.
- Supports automatic collector stop.
- Supports configurable run-on timers.
- Supports configurable start delays.
- Supports demand aggregation.
- Supports manual override.
- Supports fault handling.
- Supports multiple simultaneous demands.
- Is fully unit testable.
- Has been validated in a real workshop.

## Child Issues

- Implement collector run-on timer
- Implement collector start delay
- Implement demand aggregation
- Implement automatic collector control
- Implement manual override support
- Implement collector fault handling

## Out of Scope

- Gate control.
- Workshop routing.
- Device discovery.

## Definition of Done

The operator can walk up to a machine, switch it on, and observe the
dust collector automatically starting. When the final machine stops, the
collector remains active for the configured run-on period before
shutting down.

## References

- ADR-0014
- docs/specification/04-state-machines.md
- Milestone: v0.2.0 - Automatic Extraction
EOF

create_issue "Implement IMachineDemandSource interface" "feature" <<'EOF'
## Description

Implement the IMachineDemandSource interface used to detect extraction
demand from machines.

## Acceptance Criteria

- Supports demand_active().
- Supports diagnostics().
- Supports calibration state.
- Hardware independent.
- Unit tests included.
- CI passes.

## References

- Parent Epic: [EPIC] Machine Demand Detection
- ADR-0014
EOF

create_issue "Implement ESP32 ADC abstraction" "feature" <<'EOF'
## Description

Implement an ESP32 ADC abstraction layer.

## Acceptance Criteria

- Supports configurable ADC channel.
- Supports configurable sample rate.
- Supports raw sample acquisition.
- Hardware independent interface exposed.
- Hardware tested.
- CI passes.

## References

- Parent Epic: [EPIC] Machine Demand Detection
EOF

create_issue "Implement Current Transformer abstraction" "feature" <<'EOF'
## Description

Implement a Current Transformer abstraction for coil-based current
sensors.

## Acceptance Criteria

- Supports arbitrary turns ratios.
- Supports future Current Sensor implementations.
- Supports configurable burden resistor values.
- Supports configurable calibration factors.
- Supports persistence of calibration settings.
- Unit tests included.
- Hardware validated.
- CI passes.

## Technical Notes

Initial hardware:

- Coil-based CT.
- No integrated burden resistor.

## References

- Parent Epic: [EPIC] Machine Demand Detection
EOF

create_issue "Implement RMS current calculation" "feature" <<'EOF'
## Description

Implement RMS current calculation.

## Acceptance Criteria

- Configurable sampling window.
- Accuracy within ±5%.
- Supports multiple CT ratios.
- Unit tests included.
- Hardware validated.
- CI passes.

## References

- Parent Epic: [EPIC] Machine Demand Detection
EOF

create_issue "Implement demand threshold evaluation" "feature" <<'EOF'
## Description

Implement demand threshold evaluation.

## Acceptance Criteria

- Threshold is configurable.
- Supports hysteresis.
- Supports multiple machine profiles.
- Unit tests included.
- CI passes.

## References

- Parent Epic: [EPIC] Machine Demand Detection
EOF

create_issue "Implement demand debounce support" "feature" <<'EOF'
## Description

Implement demand debounce support.

## Acceptance Criteria

- Configurable debounce period.
- Prevents false starts.
- Prevents duplicate events.
- Unit tests included.
- CI passes.

## References

- Parent Epic: [EPIC] Machine Demand Detection
EOF

create_issue "Implement current transformer calibration" "feature" <<'EOF'
## Description

Implement CT calibration support.

## Acceptance Criteria

- Calibration factor configurable.
- Calibration persisted.
- Calibration survives reboot.
- Unit tests included.
- Hardware validated.
- CI passes.

## References

- Parent Epic: [EPIC] Machine Demand Detection
EOF

create_issue "Create current transformer test fixture" "feature" <<'EOF'
## Description

Create a repeatable CT test fixture for validation.

## Acceptance Criteria

- Supports repeatable measurements.
- Documents test procedure.
- Supports multiple current levels.
- Fixture documented.
- Results recorded.

## References

- Parent Epic: [EPIC] Machine Demand Detection
EOF

create_issue "Implement MachineDemandStarted event" "feature" <<'EOF'
## Description

Implement the MachineDemandStarted Event.

## Acceptance Criteria

- Contains Device UID.
- Contains Unit UID.
- Contains Event ID.
- Contains timestamp.
- Unit tests included.
- CI passes.

## References

- Parent Epic: [EPIC] Machine Demand Detection
EOF

create_issue "Implement MachineDemandStopped event" "feature" <<'EOF'
## Description

Implement the MachineDemandStopped Event.

## Acceptance Criteria

- Contains Device UID.
- Contains Unit UID.
- Contains Event ID.
- Contains timestamp.
- Unit tests included.
- CI passes.

## References

- Parent Epic: [EPIC] Machine Demand Detection
EOF

create_issue "Implement collector run-on timer" "feature" <<'EOF'
## Description

Implement collector run-on behavior.

## Acceptance Criteria

- Configurable duration.
- Default duration: 10 seconds.
- Timer resets on new demand.
- Unit tests included.
- CI passes.

## References

- Parent Epic: [EPIC] Collector Automation
EOF

create_issue "Implement collector start delay" "feature" <<'EOF'
## Description

Implement configurable collector start delay.

## Acceptance Criteria

- Delay configurable.
- Supports zero delay.
- Unit tests included.
- CI passes.

## References

- Parent Epic: [EPIC] Collector Automation
EOF

create_issue "Implement demand aggregation" "feature" <<'EOF'
## Description

Implement demand aggregation across all active machine Units.

## Acceptance Criteria

- Supports multiple active demands.
- Supports reference counting.
- Supports concurrent starts.
- Supports concurrent stops.
- Unit tests included.
- CI passes.

## References

- Parent Epic: [EPIC] Collector Automation
- ADR-0014
EOF

create_issue "Implement automatic collector control" "feature" <<'EOF'
## Description

Implement automatic collector control based on active machine demand.

## Acceptance Criteria

- Collector starts automatically.
- Collector stops automatically.
- Supports multiple demand sources.
- Validated in workshop.
- CI passes.

## References

- Parent Epic: [EPIC] Collector Automation
- Milestone: v0.2.0 - Automatic Extraction
EOF

create_issue "Implement manual override support" "feature" <<'EOF'
## Description

Implement manual override support.

## Acceptance Criteria

- Supports forced ON.
- Supports forced OFF.
- Supports AUTO mode.
- Current mode exposed via API.
- Unit tests included.
- CI passes.

## References

- Parent Epic: [EPIC] Collector Automation
EOF

create_issue "Implement collector fault handling" "feature" <<'EOF'
## Description

Implement collector fault handling.

## Acceptance Criteria

- Supports fault state.
- Emits fault Events.
- Prevents unsafe operation.
- Supports fault recovery.
- Unit tests included.
- CI passes.

## References

- Parent Epic: [EPIC] Collector Automation
- docs/specification/04-state-machines.md
EOF

echo "Bootstrap complete through v0.2.0."
echo "OpenExtraction should now be capable of capturing First Dust."
exit() 
###############################################################################
# Milestone: v0.3.0 - Multi-Machine Workshop MVP
###############################################################################

create_epic "[EPIC] Workshop Model" <<'EOF'
## Goal

Introduce the workshop abstraction.

## Acceptance Criteria

- Workshop model implemented.
- Devices belong to a workshop.
- Units belong to devices.
- Capabilities belong to units.
EOF

create_epic "[EPIC] Multiple Machine Support" <<'EOF'
## Goal

Support multiple simultaneous machines.

## Acceptance Criteria

- Multiple machine units supported.
- Concurrent demand handling works.
- Demand aggregation works correctly.
EOF

create_epic "[EPIC] Safety" <<'EOF'
## Goal

Ensure safe workshop operation.

## Acceptance Criteria

- Emergency stop supported.
- Safe startup implemented.
- Safe shutdown implemented.
- Fault propagation implemented.
EOF

create_issue "Implement Workshop model" "feature"
create_issue "Implement Physical Device model" "feature"
create_issue "Implement Unit model" "feature"
create_issue "Implement Capability model" "feature"
create_issue "Implement Workshop registry" "feature"

create_issue "Support multiple machine units" "feature"
create_issue "Implement machine registration" "feature"
create_issue "Implement demand reference counting" "feature"
create_issue "Implement concurrent machine handling" "feature"
create_issue "Implement machine diagnostics" "feature"

create_issue "Implement emergency stop support" "feature"
create_issue "Implement safe startup handling" "feature"
create_issue "Implement safe shutdown handling" "feature"
create_issue "Implement watchdog support" "feature"
create_issue "Implement fault propagation" "feature"

exit()

###############################################################################
# Milestone: v0.4.0 - Multi-Unit Device Model
###############################################################################

gh issue create \
    --title "[EPIC] ADR-0014 Implementation" \
    --label "epic"

gh issue create \
    --title "[EPIC] Discovery" \
    --label "epic"

gh issue create \
    --title "Implement Device Registry" \
    --label "feature"

gh issue create \
    --title "Implement Unit Registry" \
    --label "feature"

gh issue create \
    --title "Implement Capability Registry" \
    --label "feature"

gh issue create \
    --title "Implement immutable Device UID" \
    --label "feature"

gh issue create \
    --title "Implement hostname generation" \
    --label "feature"

gh issue create \
    --title "Implement Device Information service" \
    --label "feature"

gh issue create \
    --title "Implement discovery messages" \
    --label "feature"

gh issue create \
    --title "Implement discovery service" \
    --label "feature"

gh issue create \
    --title "Implement discovery timeout handling" \
    --label "feature"

gh issue create \
    --title "Implement DeviceOnline event" \
    --label "feature"

gh issue create \
    --title "Implement DeviceOffline event" \
    --label "feature"
	
###############################################################################
# Milestone: v0.5.0 - Configuration & Persistence
###############################################################################

gh issue create \
    --title "[EPIC] Configuration" \
    --label "epic"

gh issue create \
    --title "[EPIC] Persistence" \
    --label "epic"

gh issue create \
    --title "[EPIC] Pairing" \
    --label "epic"

gh issue create \
    --title "Implement Configuration Service" \
    --label "feature"

gh issue create \
    --title "Implement configuration validation" \
    --label "feature"

gh issue create \
    --title "Implement workshop configuration model" \
    --label "feature"

gh issue create \
    --title "Implement configuration versioning" \
    --label "feature"

gh issue create \
    --title "Add NVS support" \
    --label "feature"

gh issue create \
    --title "Implement calibration persistence" \
    --label "feature"

gh issue create \
    --title "Implement workshop persistence" \
    --label "feature"

gh issue create \
    --title "Implement backup and restore" \
    --label "feature"

gh issue create \
    --title "Implement configuration migration" \
    --label "feature"

gh issue create \
    --title "Implement pairing mode" \
    --label "feature"

gh issue create \
    --title "Implement pairing workflow" \
    --label "feature"

gh issue create \
    --title "Implement pairing timeout handling" \
    --label "feature"

gh issue create \
    --title "Implement pairing security checks" \
    --label "feature"
	
###############################################################################
# Milestone: v0.6.0 - Web API & Web UI
###############################################################################

gh issue create \
    --title "[EPIC] REST API" \
    --label "epic"

gh issue create \
    --title "[EPIC] Web UI" \
    --label "epic"

gh issue create \
    --title "Implement API framework" \
    --label "feature"

gh issue create \
    --title "Implement Device endpoints" \
    --label "feature"

gh issue create \
    --title "Implement Unit endpoints" \
    --label "feature"

gh issue create \
    --title "Implement Workshop endpoints" \
    --label "feature"

gh issue create \
    --title "Implement Configuration endpoints" \
    --label "feature"

gh issue create \
    --title "Implement Diagnostics endpoints" \
    --label "feature"

gh issue create \
    --title "Implement Dashboard" \
    --label "feature"

gh issue create \
    --title "Implement Workshop view" \
    --label "feature"

gh issue create \
    --title "Implement Device Management" \
    --label "feature"

gh issue create \
    --title "Implement Unit Management" \
    --label "feature"

gh issue create \
    --title "Implement Configuration UI" \
    --label "feature"

gh issue create \
    --title "Implement Diagnostics UI" \
    --label "feature"
	
###############################################################################
# Milestone: v0.7.0+ (Routing, Automation, Diagnostics)
###############################################################################

gh issue create \
    --title "[EPIC] Routing" \
    --label "epic"

gh issue create \
    --title "[EPIC] Gate Units" \
    --label "epic"

gh issue create \
    --title "[EPIC] Automation Policies" \
    --label "epic"

gh issue create \
    --title "[EPIC] Diagnostics" \
    --label "epic"

gh issue create \
    --title "[EPIC] Hardware-In-The-Loop Testing" \
    --label "epic"

gh issue create \
    --title "[EPIC] Release Preparation" \
    --label "epic"

gh issue create \
    --title "Implement Route model" \
    --label "feature"

gh issue create \
    --title "Implement Routing Service" \
    --label "feature"

gh issue create \
    --title "Implement Gate Unit" \
    --label "feature"

gh issue create \
    --title "Implement Gate State Machine" \
    --label "feature"

gh issue create \
    --title "Implement Maintenance Mode" \
    --label "feature"

gh issue create \
    --title "Implement Event Logger" \
    --label "feature"

gh issue create \
    --title "Implement HIL framework" \
    --label "feature"

gh issue create \
    --title "Publish v1.0.0" \
    --label "feature"
	
