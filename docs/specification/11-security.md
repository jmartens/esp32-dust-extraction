# Security

## Purpose

This document defines the architectural security model of the OpenExtraction system.

The Security Model specifies:

- trust relationships;
- authentication requirements;
- pairing requirements;
- authorisation requirements;
- ownership boundaries.

Cryptographic and transport-specific security mechanisms are intentionally outside the scope of this specification.

---

# Principles

The Security Model is founded upon the following principles.

- The Controller is the single trusted authority.
- Physical Devices are untrusted until paired.
- Business decisions SHALL only be made by the Controller.
- Security SHALL remain transport independent.
- Authentication and authorisation SHALL be explicit.
- Physical Devices SHALL operate with the minimum required privileges.

---

# Trust Model

The Controller SHALL be considered the sole trusted authority within the system.

The Controller SHALL determine:

- which Physical Devices are trusted;
- which Units are available;
- which Routes are valid;
- which Commands may be issued.

Physical Devices SHALL NOT be trusted until successfully paired.

---

# Physical Device Pairing

Every Physical Device SHALL undergo a pairing process before participating in automatic operation.

The pairing process SHALL:

- establish trust;
- establish device identity;
- associate a Device UID with the Controller;
- persist pairing information.

Unpaired Physical Devices SHALL NOT participate in business logic.

---

# Unit Trust

Units inherit the trust relationship of their hosting Physical Device.

Units hosted by unpaired Physical Devices SHALL NOT participate in:

- routing;
- automation;
- command execution.

The Controller MAY expose information from unpaired devices for administrative purposes.

---

# Authentication

All communication participants SHALL be authenticated.

Authentication mechanisms are transport specific and SHALL be defined by the corresponding transport specifications.

The architecture intentionally makes no assumptions regarding the authentication mechanism.

---

# Authorisation

Only the Controller SHALL be authorised to:

- perform routing decisions;
- perform automation;
- modify configuration;
- issue Commands.

Physical Devices SHALL NOT be authorised to make business decisions.

---

# Command Authorisation

Physical Devices SHALL only execute Commands originating from the Controller.

Commands SHALL be validated before execution.

Invalid or unauthorised Commands SHALL be ignored.

The validation mechanism is transport specific.

---

# Configuration Security

Configuration changes SHALL be authorised by the Controller.

External interfaces SHALL NOT directly modify Controller-owned state.

Configuration changes SHALL be validated before being applied.

---

# Resource Security

The Controller owns the Resource Namespace.

External interfaces SHALL access information exclusively through Controller-owned Resources.

Resource access policies are implementation dependent and outside the scope of this specification.

Examples include:

- read-only resources;
- authenticated resources;
- administrative resources.

---

# Discovery Security

Discovery SHALL NOT imply trust.

The discovery of a Physical Device SHALL NOT automatically:

- pair the device;
- enable automatic operation;
- modify configuration.

Pairing SHALL always be an explicit operation.

---

# Fault Isolation

The failure or compromise of one Physical Device SHALL NOT compromise:

- other Physical Devices;
- Controller-owned state;
- Controller Services.

Physical Devices SHALL remain logically isolated.

---

# Manual Operations

Manual operations SHALL remain subject to Controller authorisation.

Physical Devices SHALL NOT bypass Controller security policies.

---

# Transport Security

Transport-specific security mechanisms SHALL be defined separately.

Examples include:

- encryption;
- message authentication;
- replay protection;
- sequence numbers;
- session management.

Transport specifications SHALL remain consistent with the architectural security model.

---

# Persistence Security

Sensitive information MAY require protected storage.

Examples include:

- pairing information;
- authentication credentials;
- transport keys.

Storage mechanisms are implementation dependent.

---

# Design Principles

The Security Model SHALL satisfy the following principles.

- Controller-centric trust.
- Explicit pairing.
- Explicit authentication.
- Explicit authorisation.
- Transport independence.
- Fault isolation.
- Minimal device privileges.
- Separation of architectural and cryptographic concerns.