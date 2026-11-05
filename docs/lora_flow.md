# Communication flow

## DATA/ACK

1. The application supplies a destination and 1–32 payload bytes.
2. The link validates the length and builds the packet.
3. Encoding checks output capacity before serializing the header and payload.
4. The radio sends the bytes and returns to receive mode.
5. The receiver checks the complete packet, network and destination.
6. The receiver sends ACK with the request's packet ID.
7. The sender accepts only the expected peer's matching ACK.
8. A timeout starts another attempt, up to three total; exhaustion returns failure.

## PING/PONG

The sender transmits PING and waits for PONG from the requested node. Network,
source, destination, type and packet ID must match. The receiver reuses the
request ID in its response. This path does not add DATA retransmissions.

## Failure and timing limits

Malformed packets and unrelated responses are ignored. A missing response is a
failure, not confirmation. Polling and LED delays block the caller. The radio
driver's transmission timeout and register I/O contribute additional elapsed time;
the ACK polling budget is not a complete end-to-end timing guarantee.

Host tests simulate the radio and clock delays. Recheck TX/RX behavior on the
intended modules after changing firmware, frequency or wiring.
