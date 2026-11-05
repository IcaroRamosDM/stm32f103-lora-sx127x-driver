# Packet and link protocol

## Wire representation

| Byte | Field |
|---|---|
| 0 | Network identifier |
| 1 | Source node |
| 2 | Destination node |
| 3 | Packet identifier |
| 4 | Type: DATA=1, ACK=2, PING=3, PONG=4 |
| 5 | Payload length, 0–32 |
| 6 onward | Exactly payload_length bytes |

The maximum encoded packet is 38 bytes. The representation contains no padding.

## Packet API

lora_packet_encode(packet, buffer, capacity) returns the encoded size or zero.
It rejects NULL pointers, unsupported types, oversized payloads and insufficient
output capacity before writing the output buffer.

lora_packet_decode(packet, buffer, length) returns one for a valid frame and zero
otherwise. It rejects missing header/payload bytes, extra trailing bytes,
unsupported types and payloads longer than 32 bytes. Rejection preserves the
caller's packet. The length argument describes the actual received bytes.

Network and node filtering belong to the link layer, not the byte decoder.

## Link behavior

DATA payloads contain 1–32 bytes. The transmitter makes at most three total
attempts, waiting up to 1000 polling iterations with a 1 ms delay for an ACK
after each transmission. This is not three retries in addition to an initial attempt.

ACK and PONG matching checks the network, expected source, local destination,
packet ID and response type. The current demo uses TX node 0x01, RX node 0x02
and network 0xA1. A successful exchange advances the packet identifier.
PING uses a single request and response wait.

The receiver answers DATA with ACK and PING with PONG after address validation.
Retries may deliver DATA more than once: there is no duplicate suppression.
The header provides logical filtering, not encryption or authentication, and does
not prevent radio collisions or interference.
