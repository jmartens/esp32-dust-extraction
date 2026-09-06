#!/usr/bin/env bash
#
# bootstrap/milestones.sh
#
# Repository milestone management.
#

set -Eeuo pipefail

###############################################################################
# Milestone definitions
###############################################################################

MILESTONES=(
"v0.0.1 - Repository Bootstrap"
"v0.1.0 - First Dust"
"v0.2.0 - Automatic Extraction"
"v0.3.0 - Workshop MVP"
)

###############################################################################
# Ensure a single milestone exists
###############################################################################

ensure_milestone() {

    local title="$1"

    [[ -n "${MILESTONE_CACHE[$title]:-}" ]] && return

    if gh_query api \
        "repos/${GITHUB_REPOSITORY}/milestones" \
        --jq '.[].title' \
        | grep -Fxq "$title"
    then
        MILESTONE_CACHE["$title"]=1
        return
    fi

    info "Creating milestone '$title'"

    gh_exec api \
        "repos/${GITHUB_REPOSITORY}/milestones" \
        --method POST \
        -f title="$title"

    MILESTONE_CACHE["$title"]=1

}

###############################################################################
# Ensure all milestones exist
###############################################################################

ensure_milestones() {

    local milestone

    for milestone in "${MILESTONES[@]}"; do
        ensure_milestone "$milestone"
    done

}