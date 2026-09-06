#!/usr/bin/env bash
#
# milestones/0.1.0-first-dust.sh
#
# v0.1.0 - First Dust
#

CURRENT_MILESTONE="v0.1.0 - First Dust"
readonly CURRENT_MILESTONE

###############################################################################
# Controller
###############################################################################

begin_epic_issue \
    "Controller" \
    "Controller skeleton" \
    "feature,controller"
<<'EOF'
## Goal

Create the initial controller application.

## Acceptance criteria

- Starts successfully
- Accepts device connections
- Processes events
- Writes log output
EOF

begin_epic_issue \
    "Controller" \
    "Static route" \
    "feature,controller"
<<'EOF'
## Goal

Configure a single predefined extraction route.

## Acceptance criteria

- One machine
- One blast gate
- One collector
- Route activates correctly
EOF

###############################################################################
# Runtime
###############################################################################

begin_epic_issue \
    "Runtime" \
    "Event bus" \
    "feature,runtime"
<<'EOF'
## Goal

Implement the internal event bus.

## Acceptance criteria

- Publish events
- Subscribe to events
- Typed events
EOF

begin_epic_issue \
    "Runtime" \
    "Message protocol v1" \
    "feature,runtime"
<<'EOF'
## Goal

Implement protocol version 1.

## Acceptance criteria

- Serialize messages
- Deserialize messages
- Validate protocol version
EOF

###############################################################################
# Firmware Platform
###############################################################################

begin_epic_issue \
    "Firmware Platform" \
    "Firmware skeleton" \
    "feature,firmware"
<<'EOF'
## Goal

Provide a common firmware foundation.

## Acceptance criteria

- Boot firmware
- Logging available
- Communicate with controller
EOF

###############################################################################
# Machine Unit
###############################################################################

begin_epic_issue \
    "Machine Unit" \
    "Machine demand detection" \
    "feature,firmware"
<<'EOF'
## Goal

Detect when a machine requires dust extraction.

## Acceptance criteria

- Demand detected
- Demand cleared
- Demand event transmitted
EOF

###############################################################################
# Blast Gate Unit
###############################################################################

begin_epic_issue \
    "Blast Gate Unit" \
    "Blast gate control" \
    "feature,firmware"
<<'EOF'
## Goal

Operate a single blast gate.

## Acceptance criteria

- Open gate
- Close gate
- Report completion
EOF

###############################################################################
# Collector Unit
###############################################################################

begin_epic_issue \
    "Collector Unit" \
    "Collector control" \
    "feature,firmware"
<<'EOF'
## Goal

Operate the dust collector.

## Acceptance criteria

- Start collector
- Stop collector
- Report completion
EOF

###############################################################################
# Validation
###############################################################################

begin_epic_issue \
    "Testing & Validation" \
    "First dust demonstration" \
    "testing"
<<'EOF'
## Goal

Demonstrate successful automatic dust extraction.

## Acceptance criteria

When the configured machine starts:

- Machine demand detected
- Controller receives event
- Blast gate opens
- Collector starts
- Dust extraction works

When the machine stops:

- Collector stops after configured delay
- Blast gate closes
- System returns to idle
EOF