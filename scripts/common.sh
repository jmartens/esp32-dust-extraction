#!/usr/bin/env bash
#
# bootstrap/common.sh
#
# Shared helper functions for the OpenExtraction GitHub bootstrap.
#

set -Eeuo pipefail

###############################################################################
# Global configuration
###############################################################################

: "${GITHUB_REPOSITORY:?Please export GITHUB_REPOSITORY (owner/repo)}"

CURRENT_MILESTONE="${CURRENT_MILESTONE:-}"

DRY_RUN="${DRY_RUN:-false}"
UPDATE_EXISTING="${UPDATE_EXISTING:-false}"
VERBOSE="${VERBOSE:-false}"

LAST_EPIC=""
LAST_ISSUE=""

declare -Ag ISSUE_CACHE=()
declare -Ag EPIC_CACHE=()
declare -Ag MILESTONE_CACHE=()
declare -Ag LABEL_CACHE=()

###############################################################################
# Logging
###############################################################################

info() {
    printf '[INFO] %s\n' "$*"
}

warn() {
    printf '[WARN] %s\n' "$*" >&2
}

fatal() {
    printf '[ERROR] %s\n' "$*" >&2
    exit 1
}

debug() {
    [[ "$VERBOSE" == "true" ]] || return 0

    printf '[DEBUG] %s\n' "$*"
}

###############################################################################
# Validation
###############################################################################

require_milestone() {

    local milestone="$1"

    [[ -n "$milestone" ]] \
        || fatal "Milestone not set."

}

###############################################################################
# GitHub helpers
###############################################################################

gh_query() {

    gh "$@"

}

gh_exec() {

    if [[ "$DRY_RUN" == "true" ]]; then
        printf '[DRYRUN] gh '
        printf '%q ' "$@"
        printf '\n'
        return 0
    fi

    gh "$@"

}

###############################################################################
# Issue lookup
###############################################################################

find_issue() {

    local title="$1"

    if [[ -n "${ISSUE_CACHE[$title]:-}" ]]; then
        printf '%s\n' "${ISSUE_CACHE[$title]}"
        return
    fi

    debug "Looking up issue: $title"

    local number

    number="$(
        gh_query issue list \
            --search "in:title \"$title\"" \
            --json number,title \
            --jq '.[] | select(.title=="'"$title"'") | .number'
    )"

    if [[ -n "$number" ]]; then
        ISSUE_CACHE["$title"]="$number"
    fi

    printf '%s\n' "$number"
}

###############################################################################
# Epic lookup
###############################################################################

find_epic() {

    local title="$1"

    if [[ -n "${EPIC_CACHE[$title]:-}" ]]; then
        printf '%s\n' "${EPIC_CACHE[$title]}"
        return
    fi

    local number

    number="$(
        gh_query issue list \
            --label epic \
            --search "in:title \"$title\"" \
            --json number,title \
            --jq '.[] | select(.title=="'"$title"'") | .number'
    )"

    if [[ -n "$number" ]]; then
        EPIC_CACHE["$title"]="$number"
    fi

    printf '%s\n' "$number"

}

###############################################################################
# Label lookup
###############################################################################

find_label() {

    local name="$1"

    if [[ -n "${LABEL_CACHE[$name]:-}" ]]; then
        printf '%s\n' "${LABEL_CACHE[$name]}"
        return
    fi

    local label

    label="$(
        gh_query label list \
            --json name \
            --jq '.[] | select(.name=="'"$name"'") | .name'
    )"

    if [[ -n "$label" ]]; then
        LABEL_CACHE["$name"]="$label"
    fi

    printf '%s\n' "$label"

}

###############################################################################
# Milestone lookup
###############################################################################

find_milestone() {

    local title="$1"

    if [[ -n "${MILESTONE_CACHE[$title]:-}" ]]; then
        printf '%s\n' "${MILESTONE_CACHE[$title]}"
        return
    fi

    local number

    number="$(
        gh api \
            --paginate \
            "repos/$GITHUB_REPOSITORY/milestones?state=all&per_page=100" \
            --jq '.[] | select(.title=="'"$title"'") | .number'
    )"

    if [[ -n "$number" ]]; then
        MILESTONE_CACHE["$title"]="$number"
    fi

    printf '%s\n' "$number"

}

###############################################################################
# Generic issue creator
###############################################################################

create_issue() {

    local title="$1"
    local labels="$2"
    local milestone="$3"
    local body="$4"

    local number

    number="$(find_issue "$title")"

    if [[ -n "$number" ]]; then

        info "Issue exists (#$number): $title"

        if [[ "$UPDATE_EXISTING" == "true" ]]; then

            gh_exec issue edit \
                "$number" \
                --body "$body" \
                --add-label "$labels" \
                --milestone "$milestone" >/dev/null

        fi

        printf '%s\n' "$number"
        return

    fi

    info "Creating issue: $title"

    local url

    url="$(
        gh_exec issue create \
            --title "$title" \
            --label "$labels" \
            --milestone "$milestone" \
            --body "$body"
    )"

    if [[ "$DRY_RUN" == "true" ]]; then
        printf 'DRYRUN\n'
        return
    fi

    printf '%s\n' "${url##*/}"

}

###############################################################################
# Public DSL
###############################################################################

begin_epic() {

    local title="$1"
    local labels="$2"

    local body

    body="$(cat)"

    LAST_EPIC="$(
        create_issue \
            "$title" \
            "epic,$labels" \
            "" \
            "$body"
    )"

    EPIC_CACHE["$title"]="$LAST_EPIC"

}

begin_issue() {

    local title="$1"
    local labels="$2"
    local milestone="${3:-$CURRENT_MILESTONE}"

    require_milestone "$milestone"

    local body

    body="$(cat)"

    LAST_ISSUE="$(
        create_issue \
            "$title" \
            "$labels" \
            "$milestone" \
            "$body"
    )"

}

link_to_epic() {

    local issue="$1"
    local epic="$2"

    gh_exec issue edit \
        "$issue" \
        --parent "$epic" >/dev/null

}

begin_epic_issue() {

    local epic="$1"

    shift

    begin_issue "$@"

    local epic_number

    epic_number="$(find_epic "$epic")"

    if [[ -z "$epic_number" && "$DRY_RUN" == "true" ]]; then
        epic_number="${EPIC_CACHE[$epic]:-}"
    fi

    [[ -n "$epic_number" ]] \
        || fatal "Epic '$epic' not found."

    link_to_epic \
        "$LAST_ISSUE" \
        "$epic_number"

}