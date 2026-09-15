# Architecture Decision Records

Architecture Decision Records (ADRs) capture accepted architectural decisions
and their rationale. New architectural changes should add or update an ADR
before implementation, as described in [ADR-0001](0001-architecture-first.md).

## Directory Convention

The ADR files in this directory are the index. GitHub and local repository
directory listings provide the complete, current list without requiring a
second manually maintained table of links, titles, or statuses.

New ADRs should use a unique numeric identifier and a descriptive filename.
Each ADR should state its status and link to any specifications it changes.
Incomplete or superseded decisions remain visible in this directory until a
follow-up decision explicitly completes or retires them.

## Status and Maintenance

Use [ADR-0001](0001-architecture-first.md) as the starting point for the ADR
workflow. Architectural changes should be documented before implementation,
reviewed, accepted, and reflected in the relevant specifications.

When an ADR changes an authoritative specification, update the relevant
specification and add cross-references in both documents. ADR links should use
repository-relative paths so they remain valid in local checkouts and on
GitHub.
