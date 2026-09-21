# ESP Relay Driver

`EspRelayDriver` is the ESP-IDF adapter for the portable `relay::RelayDriver`
abstraction. It configures the supplied GPIO pin as an output and initializes
the relay in its logical off state.

Use `RelayPolarity::active_high` when a high GPIO level energizes the relay,
or `RelayPolarity::active_low` when a low level energizes it.
