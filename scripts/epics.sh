#!/usr/bin/env bash
#
# bootstrap/epics.sh
#
# Repository epic management.
#

set -Eeuo pipefail

###############################################################################
# Epic definitions
#
# Format:
#   Title|Labels
###############################################################################

EPICS=(
    "Repository Bootstrap|bootstrap"
    "Common Library|feature,common-library"
    "Runtime|feature,runtime"
    "Controller|feature,controller"
    "Firmware Platform|feature,firmware"
    "Machine Unit|feature,firmware"
    "Collector Unit|feature,firmware"
    "Blast Gate Unit|feature,firmware"
    "Hardware Drivers|feature,driver"
    "Configuration|feature,configuration"
    "Networking|feature,networking"
    "OTA Updates|feature,ota"
    "Testing & Validation|testing"
    "Documentation|documentation"
)

###############################################################################
# Ensure one epic exists
###############################################################################

ensure_epic() {

    local title="$1"
    local labels="$2"

    local number

    number="$(find_issue "$title")"

    if [[ -n "$number" ]]; then
        info "Epic exists (#$number): $title"
        EPIC_CACHE["$title"]="$number"
        return
    fi

    info "Creating epic '$title'"

    begin_epic \
        "$title" \
        "$labels" \
<<EOF
## Summary

Epic tracking all work related to **${title}**.

## Scope

To be refined as implementation progresses.
EOF

    EPIC_CACHE["$title"]="$LAST_EPIC"

}

###############################################################################
# Ensure all epics exist
###############################################################################

ensure_epics() {

    local epic
    local title
    local labels

    for epic in "${EPICS[@]}"; do

        IFS='|' read -r \
            title \
            labels \
            <<<"$epic"

        ensure_epic \
            "$title" \
            "$labels" \

    done

}