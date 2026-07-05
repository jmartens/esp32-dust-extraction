# Persistence

## Purpose

This document defines the persistence model of the OpenExtraction Controller.

The Persistence Model specifies:

- which information is persisted;
- persistence requirements;
- information lifetimes;
- ownership of persistent information.

The Persistence Model intentionally makes no assumptions regarding storage mechanisms or file formats.

---

# Principles

The Persistence Model is founded upon the following principles.

- Persistence is a policy decision.
- Information ownership SHALL remain explicit.
- Persistence SHALL remain transport independent.
- Persistence SHALL remain storage independent.
- Only information requiring persistence SHALL be persisted.

---

# Information Categories

Information MAY belong to one of the following persistence categories.

- Persistent
- Runtime Only
- Optional Persistence

The owning Service SHALL determine the persistence category of the information it owns unless otherwise specified.

---

# Persistent Information

Persistent information SHALL survive Controller restarts.

Examples include:

- configuration;
- route definitions;
- pairing information;
- calibration values;
- friendly names;
- user metadata.

Persistent information SHALL be restored during Controller startup.

---

# Runtime Information

Runtime information SHALL NOT survive Controller restarts.

Examples include:

- active Routes;
- Gate reference counts;
- pending Commands;
- active timers;
- Event Queues.

Runtime information SHALL be reconstructed during normal Controller operation.

---

# Optional Persistence

Optional persistence MAY be used for information where retention is desirable but not required.

Examples include:

- statistics;
- fault history;
- diagnostics;
- communication counters.

The implementation MAY choose whether such information is persisted.

---

# Ownership

Persistent information SHALL remain owned by the owning Controller Service.

Examples include:

| Information | Owner |
|------------|------------|
| Configuration | Configuration Service |
| Route definitions | Configuration Service |
| Pairing information | Device Registry |
| Diagnostics | Diagnostics Service |
| Statistics | Diagnostics Service |

Persistence SHALL NOT transfer ownership.

---

# Controller Startup

Persistent information SHALL be restored during Controller startup.

Examples include:

- configuration;
- paired devices;
- route definitions;
- calibration values.

The Controller SHALL NOT begin automatic operation until mandatory persistent information has been restored successfully.

---

# Controller Shutdown

The Controller SHALL ensure that persistent information is written safely before shutdown where applicable.

Unexpected power loss SHALL NOT corrupt mandatory persistent information.

The implementation SHALL determine how this requirement is satisfied.

---

# Dynamic Persistence

Persistent information MAY be modified during runtime.

Configuration changes SHALL:

- be validated;
- be applied;
- be persisted.

Persistent information SHALL remain internally consistent.

---

# Resource Representation

Persistent information MAY be exposed through Controller-owned Resources.

Examples include:

```text
/configuration

/routes

/devices
```

Persistence policies SHALL NOT influence the Resource Model.

---

# Security Considerations

Sensitive information MAY require protected storage.

Examples include:

- pairing information;
- authentication credentials;
- transport-specific secrets.

The implementation SHALL determine appropriate storage mechanisms.

---

# Persistence Requirements

The following information SHALL be persistent.

```text
Configuration

YES

Pairing Information

YES

Route Definitions

YES

Friendly Names

YES

Calibration Information

YES
```

The following information SHALL NOT be persistent.

```text
Active Routes

NO

Event Queue

NO

Gate Reference Counts

NO

Pending Commands

NO

Run-On Timers

NO
```

The following information MAY be persistent.

```text
Statistics

OPTIONAL

Diagnostics

OPTIONAL

Fault History

OPTIONAL
```

---

# Storage Independence

The Persistence Model intentionally makes no assumptions regarding:

- filesystems;
- databases;
- flash storage;
- network storage;
- storage formats.

Persistence mechanisms are implementation specific.

---

# Design Principles

The Persistence Model SHALL satisfy the following principles.

- Explicit ownership.
- Persistence as policy.
- Storage independence.
- Transport independence.
- Clear information lifetimes.
- Safe restoration during startup.
- Minimal persistence requirements.