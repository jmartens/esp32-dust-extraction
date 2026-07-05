# Design Principles

## Controller Authority

Only the controller decides:

- which gates open
- which gates close
- when extraction starts
- when extraction stops

## Distributed Execution

Execution is distributed.

Decision making is centralized.

## Immutable Device Identity

Every ESP generates a unique identity.

The identity is based on:

```
<TYPE>-<MAC ADDRESS>
```

Example:

```
MACHINE-7C9EBD002341
```

The firmware never contains installation-specific names.

## Friendly Names

Friendly names are stored only in controller configuration.

Example:

```
MACHINE-7C9EBD002341

↓

Table Saw
```

## Hardware Independence

Machine firmware should not know:

- shop layout
- blast gates
- duct routing

Gate firmware should not know:

- machines
- extraction routes

Only the controller contains system knowledge.

## Safe Startup

After power loss:

- collector remains OFF
- gates remain unchanged
- devices reconnect
- controller rebuilds system state

No automatic collector restart occurs.