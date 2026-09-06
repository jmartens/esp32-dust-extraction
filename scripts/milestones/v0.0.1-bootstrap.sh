#!/usr/bin/env bash
#
# milestones/0.0.1-bootstrap.sh
#
# v0.0.1 - Repository Bootstrap
#

CURRENT_MILESTONE="v0.0.1 - Repository Bootstrap"
readonly CURRENT_MILESTONE

###############################################################################
# Repository
###############################################################################

begin_epic_issue \
    "Repository Bootstrap" \
    "Repository structure" \
    "bootstrap"
<<'EOF'
## Goal

Create the initial repository layout.

## Acceptance criteria

- Standard project directory layout
- Documentation directory
- ADR directory
- Bootstrap framework directory
- GitHub configuration directory
EOF

begin_epic_issue \
    "Repository Bootstrap" \
    "Repository documentation" \
    "bootstrap,documentation"
<<'EOF'
## Goal

Provide the initial project documentation.

## Acceptance criteria

- README.md
- CONTRIBUTING.md
- ARCHITECTURE.md
- Documentation index
EOF

begin_epic_issue \
    "Repository Bootstrap" \
    "GitHub repository configuration" \
    "bootstrap"
<<'EOF'
## Goal

Configure the GitHub repository.

## Acceptance criteria

- Branch protection
- Issue templates
- Pull request template
- CODEOWNERS
- Repository settings documented
EOF

###############################################################################
# CI / Quality
###############################################################################

begin_epic_issue \
    "Repository Bootstrap" \
    "Continuous Integration pipeline" \
    "bootstrap,testing"
<<'EOF'
## Goal

Introduce automated validation.

## Acceptance criteria

- GitHub Actions configured
- Build workflow
- Documentation validation
- Static analysis workflow
EOF

begin_epic_issue \
    "Repository Bootstrap" \
    "Code quality tooling" \
    "bootstrap,testing"
<<'EOF'
## Goal

Provide a consistent development experience.

## Acceptance criteria

- pre-commit configured
- ShellCheck
- markdownlint
- yamllint
- clang-format
- clang-tidy
- cppcheck
- GitHub CodeQL
EOF

begin_epic_issue \
    "Repository Bootstrap" \
    "Release automation" \
    "bootstrap"
<<'EOF'
## Goal

Automate project releases.

## Acceptance criteria

- Semantic Versioning
- Conventional Commits
- Automatic changelog generation
- GitHub Release workflow
EOF

###############################################################################
# Documentation
###############################################################################

begin_epic_issue \
    "Documentation" \
    "Project introduction" \
    "documentation"
<<'EOF'
## Goal

Document the vision, goals and scope of OpenExtraction.

## Acceptance criteria

- Vision
- Goals
- Scope
- Non-goals
- Intended audience
EOF

begin_epic_issue \
    "Documentation" \
    "Terminology" \
    "documentation"
<<'EOF'
## Goal

Establish a common vocabulary.

## Acceptance criteria

- Devices
- Units
- Events
- Commands
- Routes
- States
EOF

begin_epic_issue \
    "Documentation" \
    "Initial ADR set" \
    "documentation"
<<'EOF'
## Goal

Publish the initial architectural decisions.

## Acceptance criteria

- ADRs committed
- Indexed from documentation
- Cross-referenced where appropriate
EOF

###############################################################################
# Bootstrap tooling
###############################################################################

begin_epic_issue \
    "Repository Bootstrap" \
    "GitHub bootstrap framework" \
    "bootstrap"
<<'EOF'
## Goal

Create an idempotent GitHub repository bootstrap framework.

## Acceptance criteria

- Labels managed
- Milestones managed
- Epics managed
- Issues managed
- Parent relationships managed
- Validation script available
EOF

begin_epic_issue \
    "Repository Bootstrap" \
    "Bootstrap validation" \
    "bootstrap,testing"
<<'EOF'
## Goal

Verify that the bootstrap framework is reliable.

## Acceptance criteria

- Bootstrap is idempotent
- Validation reports a clean repository
- Dry-run mode verified
- Update mode verified
EOF