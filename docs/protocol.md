# Protocol Layer

## Overview

This project implements a structured LoRa packet protocol instead of sending anonymous raw bytes.

The protocol layer is responsible for:

* packet definition
* payload encoding
* payload decoding
* node addressing
* reliable delivery
* ACK handling
* retry mechanism
* Ping / Pong communication

This transforms the radio communication from:

```text id="o6ukzj"
send bytes
```

into:

```text id="0pqzqk"
send validated packets
```

which is required for real embedded systems.

---

## Protocol Layers

The protocol is divided into:

```text id="xz6uyz"
lora_packet
+
lora_link
```

Each layer has a single responsibility.

---

## `lora_packet`

Responsible for:

* packet structure definition
* packet serialization
* packet deserialization
* packet validation

This layer defines what a valid packet is.

---

## `lora_link`

Responsible for:

* ACK
* timeout
* retry
* delivery confirmation
* Ping / Pong logic

This layer defines how reliable communication happens.

---

## Packet Structure

Every transmission uses the following structure:

```text id="tgm0sq"
+------------+
| network_id |
+------------+
| source_id  |
+------------+
| dest_id    |
+------------+
| packet_id  |
+------------+
| type       |
+------------+
| length     |
+------------+
| payload    |
+------------+
```

This guarantees that every message contains enough information for validation and routing.

---

## Field Definitions

---

### `network_id`

Defines which logical network the packet belongs to.

Example:

```c id="c67l1t"
#define LORA_NETWORK_ID  0xA1U
```

Only nodes using the same `network_id` should communicate.

This prevents interference between independent LoRa systems operating in the same physical environment.

---

### `source_id`

Defines which node sent the packet.

Example:

```c id="jlwmcz"
source_id = 0x01
```

This allows the receiver to know who originated the message.

This is required for ACK responses.

---

### `destination_id`

Defines which node should receive the packet.

Example:

```c id="8l4v7u"
destination_id = 0x02
```

Only the correct destination should process the packet.

Other nodes must ignore it.

This allows:

```text id="j4m44t"
TX-A → RX-A
TX-B → RX-B
```

without collisions in application logic.

---

### `packet_id`

Defines a unique identifier for the transmission.

Example:

```c id="ujx0v0"
packet_id = 42
```

This is used for:

* ACK validation
* duplicate protection (future)
* retry confirmation

The ACK must return the same `packet_id`.

This is critical.

---

### `packet_type`

Defines the meaning of the packet.

Current supported types:

```c id="0skvt9"
DATA
ACK
PING
PONG
```

This allows the receiver to know how to process the message.

Example:

```text id="rwbpd8"
PING → respond with PONG
ACK → confirm delivery
DATA → process payload
```

---

### `payload_length`

Defines how many bytes exist inside the payload.

This prevents invalid reads and guarantees safe decoding.

---

### `payload`

Contains the real application data.

Examples:

```text id="t8lk9w"
HELLO
sensor value
configuration command
status request
telemetry frame
```

The protocol layer does not care what the payload means.

It only guarantees safe delivery.

---

## ACK Flow

Reliable delivery uses ACK confirmation.

Flow:

```text id="h9i74h"
TX sends DATA
↓
RX receives DATA
↓
RX sends ACK
↓
TX validates ACK
↓
delivery confirmed
```

If ACK is not received:

```text id="k4ov0a"
timeout
↓
retry
↓
failure
```

This prevents silent packet loss.

---

## Retry Mechanism

The project uses:

```text id="x3tlnv"
timeout + retry
```

instead of assuming the radio always works.

Example:

```text id="h8m3fe"
send packet
wait 1000 ms
if no ACK:
    retry
maximum 3 retries
```

This is required because LoRa communication is inherently unreliable in real environments.

Without retry, packet loss becomes invisible.

---

## Why ACK Does Not Depend on Payload

ACK validates:

```text id="xknk5p"
delivery
```

not:

```text id="3nmvrz"
payload meaning
```

This is important.

Even if the payload changes from:

```text id="l1j1p9"
HELLO
```

to:

```text id="wea7rw"
sensor configuration
```

the ACK logic remains the same.

This keeps the architecture modular.

Radio parameters and payload semantics do not affect transport reliability.

---

## Ping / Pong Flow

ACK confirms delivery.

Ping / Pong confirms application response.

Flow:

```text id="yc7flf"
TX sends PING
↓
RX receives PING
↓
RX sends PONG
↓
TX validates response
```

This demonstrates:

```text id="mx0jlwm"
request
→ response
```

instead of only:

```text id="ns8k79"
delivery confirmation
```

This is much closer to real embedded systems.

Examples:

* remote configuration
* heartbeat check
* node discovery
* status polling

This is why Ping / Pong is stronger than only ACK.

---

## Broadcast Support (Future)

Future implementation may use:

```c id="yw4a2y"
destination_id = 0xFF
```

for broadcast packets.

This would allow:

```text id="2i1cnr"
one TX
→ multiple RX nodes
```

Examples:

* network discovery
* synchronized commands
* telemetry broadcast

This is intentionally prepared by the current packet structure.

---

## Why Raw Bytes Are Not Enough

Sending:

```text id="2vf4vz"
HELLO
```

does not define:

* who sent it
* who should receive it
* whether it belongs to this network
* whether it requires ACK
* what type of message it is

This is not a protocol.

It is only byte transmission.

The packet layer solves this problem.

---

## Design Goal

The objective is not only:

```text id="0k32t9"
make LoRa transmit bytes
```

The real objective is:

```text id="sqe4t8"
build a reusable and reliable embedded communication protocol
```

that can scale to real applications.
