#!/usr/bin/env bash
#
# bootstrap/labels.sh
#
# Repository label management
#

set -Eeuo pipefail

###############################################################################
# Label definitions
###############################################################################

LABELS=(
"epic|5319E7|Parent issue grouping related work"
"feature|0052CC|New functionality"
"bug|D73A4A|Defect"
"documentation|0075CA|Documentation improvements"
"testing|1D76DB|Tests and validation"

"bootstrap|BFDADC|Repository bootstrap"
"framework|6F42C1|Framework implementation"
"common-library|5319E7|Shared platform independent code"
"runtime|C2E0C6|Runtime framework"
"controller|0E8A16|Controller subsystem"
"firmware|FBCA04|Firmware implementation"
"driver|F9D0C4|Hardware drivers"

"configuration|C5DEF5|Configuration management"
"networking|D4C5F9|Networking"
"ota|E4E669|Firmware updates"

"priority:high|B60205|High priority"
"priority:medium|FBCA04|Medium priority"
"priority:low|0E8A16|Low priority"
)

###############################################################################
# Ensure one label
###############################################################################

ensure_label() {

    local name="$1"
    local color="$2"
    local description="$3"

    local current

    current="$(
        gh_query label list \
            --json name,color,description \
            --jq \
            '.[] | select(.name=="'"$name"'")'
    )"

    if [[ -z "$current" ]]; then

        info "Creating label '$name'"

        gh_exec label create \
            "$name" \
            --color "$color" \
            --description "$description"

        return
    fi

    local current_color
    local current_description

    current_color="$(jq -r '.color' <<<"$current")"
    current_description="$(jq -r '.description' <<<"$current")"

    if [[ "$current_color" != "$color" ]] ||
       [[ "$current_description" != "$description" ]]
    then

        info "Updating label '$name'"

        gh_exec label edit \
            "$name" \
            --color "$color" \
            --description "$description"

    fi

}

###############################################################################
# Ensure all labels
###############################################################################

ensure_labels() {

    local label

    for label in "${LABELS[@]}"; do

        IFS='|' read -r \
            name \
            color \
            description \
            <<<"$label"

        ensure_label \
            "$name" \
            "$color" \
            "$description"

    done

}