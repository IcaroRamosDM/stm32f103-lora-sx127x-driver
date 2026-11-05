# Architecture

| Layer | Files | Responsibility |
|---|---|---|
| Application | src/app/app_lora_demo.c | Select DATA/ACK or PING/PONG behavior |
| Link | src/protocol/lora_link.c | Addressing, response matching and bounded attempts |
| Packet | src/protocol/lora_packet.c | Encode/decode a bounded wire representation |
| Radio | src/drivers/sx127x/sx127x.c | Registers, FIFO, modes and IRQ polling |
| Bus | src/drivers/spi/spi_bus.c | SPI transactions |
| Board | src/bsp/ | Clocks, pins, reset and millisecond delays |

The application owns its payload. Packet encoding copies into a caller-owned
buffer; decoding validates the complete frame before replacing the caller's packet.
The link keeps an 8-bit packet identifier and resets it during initialization.
An identifier wraps after 256 successful exchanges; it is not a globally unique ID.

Polling and LED delays are blocking. DIO0 interrupts, duplicate suppression and
multi-node scheduling are not implemented. The former empty lora.c/lora.h
placeholders were removed; the packet and link modules contain the protocol.
