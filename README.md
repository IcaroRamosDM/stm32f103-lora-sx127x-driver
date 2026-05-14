# STM32F103 + SX127x LoRa Driver

Bare-metal LoRa communication stack for STM32F103C8T6 (Blue Pill) using libopencm3 and Semtech SX1276/SX1278.

This project demonstrates a complete reusable LoRa architecture built from scratch, including:

* SPI1 bring-up
* Full SX127x register access
* LoRa mode configuration
* Reliable TX/RX communication
* Packet protocol layer
* ACK + retry mechanism
* Network / Node addressing
* Ping / Pong bidirectional demo
* Professional firmware layering (BSP + Driver + Protocol + App)

This repository is intended for:

* Embedded systems portfolio
* Firmware engineering study
* Bare-metal ARM Cortex-M learning
* LoRa architecture reference for future projects

---

## Features

### Core firmware

* STM32F103C8T6 bare-metal firmware
* libopencm3 framework
* Custom linker script
* Custom startup file
* Manual vector table configuration
* SysTick timing base
* GPIO abstraction layer
* Onboard LED control (PC13)

### Radio architecture

* SPI1 master configuration
* Manual NSS (chip select)
* Full-duplex SX127x register access
* SX127x initialization sequence
* LoRa mode setup
* Frequency configuration (915 MHz)
* Sleep / Standby / TX / RX modes
* Continuous RX mode

### Protocol layer

* Structured packet format
* `network_id`
* `source_id`
* `destination_id`
* `packet_id`
* Packet types:

  * DATA
  * ACK
  * PING
  * PONG

### Reliability layer

* ACK confirmation
* Timeout handling
* Retry mechanism
* Delivery confirmation
* Reliable packet delivery
* Ping / Pong request-response flow

---

## Hardware

### Target boards

* 2x STM32F103C8T6 (Blue Pill)

### LoRa modules

* 2x SX1276 / SX1278 modules

### Programming interface

* ST-Link V2

Optional:

* Breadboard
* Jumper wires
* USB-TTL (future debug expansion)

Important:

Always connect the antenna before powering the LoRa module.

Use:

```text
3.3V only
```

Never use:

```text
5V
```

---

## Wiring

### Blue Pill + SX127x

| STM32 | Function | SX127x |
| ----- | -------- | ------ |
| PA5   | SCK      | SCK    |
| PA6   | MISO     | MISO   |
| PA7   | MOSI     | MOSI   |
| PA4   | NSS      | NSS    |
| PB0   | RESET    | RST    |
| PB1   | DIO0     | DIO0   |
| 3V3   | Power    | VCC    |
| GND   | GND      | GND    |

More details:

See:

```text
docs/wiring.md
```

---

## Build

### Requirements

* arm-none-eabi-gcc
* libopencm3
* st-flash
* openocd

### TX node

```bash
make tx
make flash-tx
```

### RX node

```bash
make rx
make flash-rx
```

### Reflash

```bash
make reflash-tx
make reflash-rx
```

---

## Validation Demo

### Reliable delivery

```text
TX sends DATA
→ RX receives DATA
→ RX sends ACK
→ TX confirms delivery
```

This validates:

* packet delivery
* ACK handling
* retry mechanism
* delivery confirmation

### Ping / Pong

```text
TX sends PING
→ RX receives PING
→ RX sends PONG
→ TX validates response
```

This validates:

* bidirectional communication
* request / response flow
* protocol correctness
* application-level communication

### Node addressing

Example:

```text
network_id     = 0xA1
source_id      = 0x01
destination_id = 0x02
packet_type    = PING
```

This validates:

* multiple nodes
* isolated communication
* directed packets
* scalable network design

---

## Example

Demo sequence:

```text
TX → PING
RX → PONG
TX → ACK validation
→ loop
```

This confirms:

* SPI communication
* SX127x initialization
* LoRa radio configuration
* packet protocol integrity
* reliable delivery
* reusable architecture

---

## Project Structure

```text
include/
├── app/
├── bsp/
├── drivers/
│   ├── spi/
│   ├── sx127x/
│   └── lora/
└── protocol/
    ├── lora_packet.h
    └── lora_link.h

src/
├── app/
├── bsp/
├── drivers/
│   ├── spi/
│   ├── sx127x/
│   └── lora/
└── protocol/
    ├── lora_packet.c
    └── lora_link.c

startup/
linker/
docs/
```

---

## Documentation

Detailed technical explanations:

* `docs/wiring.md`
* `docs/architecture.md`
* `docs/lora_flow.md`
* `docs/protocol.md`
* `docs/sx127x.md`

These documents explain:

* LoRa communication flow
* SX127x register model
* ACK + retry architecture
* packet layer design
* network / node addressing
* Ping / Pong strategy
* hardware validation methodology
* firmware architecture decisions

---

## License

MIT License

---

## Author

**Icaro Ramos Rodrigues dos Santos**

Embedded Systems | Firmware | C | Hardware Design | STM32 | ARM Cortex-M | Linux | Verilog | UVM | Design Verification<br><br>

GitHub: https://github.com/IcaroRamosDM<br>
LinkedIn: https://ww.linkedin.com/in/icaro-ramos-r/<br>
icaroelt@gmail.com
