# LoRa Communication Flow

## Overview

This document explains the complete communication path used in the project, from application request to radio transmission and response validation.

The goal is to show how the system moves from:

```text id="a1b2c3"
function call
```

to:

```text id="d4e5f6"
validated bidirectional communication
```

across all firmware layers.

This is the practical execution flow of the architecture.

---

## High-Level Flow

```text id="g7h8i9"
App
↓
lora_link
↓
lora_packet
↓
SX127x driver
↓
SPI driver
↓
STM32 hardware
↓
LoRa air interface
↓
remote node
```

Then the response follows the reverse path.

This happens for both:

* reliable DATA delivery
* Ping / Pong request-response flow

---

# Reliable Delivery Flow

## TX Side

### Step 1 — App requests transmission

The application calls:

```c id="j1k2l3"
lora_link_send_reliable(...)
```

The app does not manage:

* retries
* ACK waiting
* timeout
* packet validation

This belongs to the protocol layer.

---

### Step 2 — `lora_link` creates the packet

The protocol layer fills:

```text id="m4n5o6"
network_id
source_id
destination_id
packet_id
packet_type
payload_length
payload
```

Example:

```text id="p7q8r9"
network_id     = 0xA1
source_id      = 0x01
destination_id = 0x02
packet_type    = DATA
```

Now the message is a real packet.

---

### Step 3 — `lora_packet` encodes bytes

The packet is serialized into a byte buffer:

```text id="s1t2u3"
header + payload
```

This is what the radio actually transmits.

---

### Step 4 — SX127x enters TX mode

The driver:

* writes FIFO
* sets payload length
* clears IRQ flags
* switches to TX mode

Example:

```c id="v4w5x6"
sx127x_send_packet(...)
```

The radio now transmits over the air.

---

### Step 5 — TX waits for ACK

After transmission:

```text id="y7z8a9"
TX → RX mode
```

and waits:

```text id="b1c2d3"
timeout
+
ACK validation
```

This prevents silent failures.

---

# RX Side

## Step 6 — RX receives packet

The receiver remains in:

```text id="e4f5g6"
continuous RX mode
```

using:

```c id="h7i8j9"
sx127x_start_receive()
```

When a valid packet arrives:

```text id="k1l2m3"
RX_DONE flag
```

is raised by the radio.

---

## Step 7 — Read FIFO safely

The receiver uses:

```text id="n4o5p6"
FIFO_RX_CURRENT_ADDR
```

instead of always reading from:

```text id="q7r8s9"
0x00
```

This is critical.

Without this, packet reading becomes unreliable.

---

## Step 8 — Decode packet

The received bytes are converted back into:

```text id="t1u2v3"
lora_packet_t
```

using:

```c id="w4x5y6"
lora_packet_decode(...)
```

Now the system knows:

* who sent it
* who should receive it
* what type it is

instead of only seeing raw bytes.

---

## Step 9 — Validate packet

The receiver checks:

```text id="z7a8b9"
network_id
destination_id
packet_type
```

Invalid packets are ignored.

This prevents unrelated nodes from interfering.

---

## Step 10 — Send ACK

If the packet is valid:

```text id="c1d2e3"
RX sends ACK
```

using:

```c id="f4g5h6"
lora_link_send_ack(...)
```

The ACK uses:

```text id="i7j8k9"
same packet_id
```

which is required for validation.

---

## Step 11 — TX validates ACK

The transmitter receives:

```text id="l1m2n3"
ACK(id = same packet_id)
```

and confirms:

```text id="o4p5q6"
delivery success
```

Only now is the transmission considered complete.

---

# Retry Flow

If ACK is not received:

```text id="r7s8t9"
timeout
↓
retry
↓
retry
↓
failure
```

Example:

```text id="u1v2w3"
max retries = 3
timeout = 1000 ms
```

This prevents invisible packet loss.

LoRa must never assume perfect delivery.

---

# Ping / Pong Flow

ACK confirms:

```text id="x4y5z6"
delivery
```

Ping / Pong confirms:

```text id="a7b8c9"
application response
```

This is stronger.

---

## TX Side

The transmitter calls:

```c id="d1e2f3"
lora_link_send_ping(...)
```

which sends:

```text id="g4h5i6"
PING
```

and waits for:

```text id="j7k8l9"
PONG
```

---

## RX Side

The receiver validates:

```text id="m1n2o3"
packet_type == PING
```

then sends:

```text id="p4q5r6"
PONG
```

using the same `packet_id`.

---

## Final Validation

TX confirms:

```text id="s7t8u9"
PONG received
```

This demonstrates:

```text id="v1w2x3"
request
→ response
```

instead of only:

```text id="y4z5a6"
delivery confirmation
```

This is much closer to real embedded systems.

---

# Why Polling Works

This project currently uses:

```text id="b7c8d9"
polling
```

instead of:

```text id="e1f2g3"
interrupts
```

Example:

```text id="h4i5j6"
check IRQ flags
wait
check again
```

This was intentional.

Reasons:

* simpler debugging
* deterministic validation
* easier bring-up
* reduced complexity during development

For portfolio and architecture demonstration, this is the correct first step.

---

# Future IRQ Migration

A future advanced version should use:

```text id="k7l8m9"
DIO0 interrupt
```

instead of polling.

This enables:

* lower latency
* lower power consumption
* better scalability
* cleaner event-driven communication

The current architecture already supports this migration.

No rewrite is required.

Only the radio event handling changes.

---

# Failure Path

If communication fails:

```text id="n1o2p3"
no ACK
no PONG
timeout
retry limit reached
```

the system reports failure.

This is important because:

```text id="q4r5s6"
silence is not success
```

Reliable systems must detect failure explicitly.

---

# Design Goal

The objective is not only:

```text id="t7u8v9"
make LoRa send bytes
```

The real objective is:

```text id="w1x2y3"
build a predictable and reliable communication flow
```

from application request to validated response.
