#!/usr/bin/env bash
#
# bootstrap/bootstrap.sh
#
# OpenExtraction repository bootstrap.
#

set -Eeuo pipefail

###############################################################################
# Locate bootstrap directory
###############################################################################

SCRIPT_DIR="$(
    cd "$(dirname "${BASH_SOURCE[0]}")"
    pwd
)"

###############################################################################
# Load framework
###############################################################################

source "${SCRIPT_DIR}/common.sh"
source "${SCRIPT_DIR}/labels.sh"
source "${SCRIPT_DIR}/milestones.sh"
source "${SCRIPT_DIR}/epics.sh"

###############################################################################
# Parse command line
###############################################################################

SHOW_HELP=false

while [[ $# -gt 0 ]]; do

    case "$1" in

        --dry-run)
            DRY_RUN=true
            ;;

        --update)
            UPDATE_EXISTING=true
            ;;

        --verbose)
            VERBOSE=true
            ;;

        --help|-h)
            SHOW_HELP=true
            ;;

        *)
            fatal "Unknown option '$1'"
            ;;

    esac

    shift

done

###############################################################################
# Help
###############################################################################

if [[ "$SHOW_HELP" == "true" ]]; then

cat <<EOF
Usage

bootstrap.sh [options]

Options

    --dry-run
        Show what would change.

    --update
        Update existing issues, labels and milestones.

    --verbose
        Enable verbose logging.

    --help
        Show this help.

EOF

exit 0

fi

###############################################################################
# Bootstrap repository metadata
###############################################################################

# info "Ensuring labels..."
ensure_labels

# info "Ensuring milestones..."
ensure_milestones

#info "Ensuring epics..."
ensure_epics

###############################################################################
# Execute milestone scripts
###############################################################################

for script in "${SCRIPT_DIR}"/milestones/v*.sh; do

    info "Executing $(basename "$script")"

    # Run each milestone inside a single child bash process that loads the
    # helper scripts and then sources the milestone file in the same process.
    # This ensures heredoc stdin attached to the milestone is consumed by the
    # same shell that has the helper functions available.
    bash -c "
        set -Eeuo pipefail
        DRY_RUN='${DRY_RUN:-false}'
        UPDATE_EXISTING='${UPDATE_EXISTING:-false}'
        VERBOSE='${VERBOSE:-false}'
        source '${SCRIPT_DIR}/common.sh'
        source '${SCRIPT_DIR}/labels.sh'
        source '${SCRIPT_DIR}/milestones.sh'
        source '${SCRIPT_DIR}/epics.sh'
        # shellcheck disable=SC1090
        source '${script}'
    " || true

done

###############################################################################
# Finished
###############################################################################

info "Bootstrap completed."
