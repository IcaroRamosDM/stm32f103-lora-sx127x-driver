# Project Architecture

## Overview

This project implements a reusable LoRa communication stack for STM32F103C8T6 using libopencm3 and the Semtech SX1276/SX1278 transceiver.

The implementation is divided into independent layers to separate:

* hardware access
* radio control
* packet abstraction
* reliable communication
* application behavior

The final example uses a Ping / Pong bidirectional communication demo with ACK + retry validation.

This architecture allows the LoRa stack to be reused for future projects such as:

* telemetry systems
* sensor networks
* industrial remote monitoring
* multi-node communication
* long-range embedded communication systems

without rewriting the radio driver.

---

## Layered Structure

```text id="fph3zj"
main
↓
app
↓
lora_link
↓
lora_packet
↓
SX127x driver
↓
generic SPI bus driver
↓
BSP SPI
↓
STM32 hardware
```

Each layer has a single responsibility.

---

## Layer Responsibilities

---

### `main.c`

Responsible for:

* system initialization
* startup sequencing
* radio initialization
* infinite application loop

It does not contain protocol logic.

Example:

```c id="34ij4d"
clock_setup();
gpio_setup();
spi_bus_init();

sx127x_init();
sx127x_set_lora_mode();
sx127x_set_frequency(...);

app_lora_demo_init();

while (1)
{
    app_lora_demo_run();
}
```

This keeps the firmware entry point minimal and predictable.

---

### `app/`

Responsible for:

* demonstration logic
* TX/RX validation
* Ping / Pong demonstration
* visible communication verification

Examples:

* reliable delivery demo
* ACK validation
* Ping / Pong validation

The application layer uses the public protocol API only.

It does not access radio registers directly.

---

### `protocol/lora_link/`

Responsible for:

* ACK handling
* retry mechanism
* timeout handling
* delivery confirmation
* Ping / Pong request-response flow

Public API examples:

```c id="5a4wfj"
lora_link_send_reliable();
lora_link_send_ping();
lora_link_receive_ping_and_respond();
```

This layer provides reliable communication.

The application should not implement retry logic.

---

### `protocol/lora_packet/`

Responsible for:

* packet structure definition
* payload encoding
* payload decoding
* packet validation
* node addressing

Packet structure:

```text id="2t9xjp"
network_id
source_id
destination_id
packet_id
packet_type
payload_length
payload
```

This layer converts raw bytes into a real protocol.

Without this layer, the radio would only transmit anonymous data.

---

### `drivers/sx127x/`

Responsible for:

* SX127x register control
* LoRa mode configuration
* frequency setup
* radio mode switching
* TX / RX operation

Public API examples:

```c id="oluh1e"
sx127x_set_lora_mode();
sx127x_send_packet();
sx127x_receive_packet();
sx127x_start_receive();
```

This layer converts register access into reusable radio APIs.

The application should never manipulate raw registers directly.

---

### `drivers/spi/`

Responsible for:

* generic SPI transactions
* register reads
* register writes
* reusable peripheral communication API

Public API examples:

```c id="4x0j4a"
spi_bus_write_register();
spi_bus_read_register();
```

This layer does not know which peripheral is connected.

It only provides SPI transport.

---

### `bsp/`

Responsible for:

* RCC clock enable
* GPIO configuration
* SPI peripheral configuration
* chip select control
* reset pin control
* DIO0 pin access
* SysTick timing
* onboard LED control

Examples:

```c id="pmd1w6"
spi_setup();
spi_transfer_byte();
spi_cs_select();
gpio_led_toggle();
```

This is the only layer that directly touches MCU hardware configuration.

---

## Why Separate BSP and Driver

A common bad practice is coupling protocol logic directly inside hardware setup.

Example of poor design:

```c id="8d4mjq"
spi_setup();
send_lora_packet();
```

inside the same module.

This project separates:

```text id="azxocg"
hardware configuration
≠
device protocol
```

This improves:

* reuse
* testing
* readability
* maintenance

The same SPI bus layer can later be reused by other peripherals such as:

* external flash
* sensors
* DAC/ADC devices
* display controllers

without changing the BSP layer.

---

## Why ACK Is Not Inside App

A common bad practice is implementing reliability directly inside `main.c`.

Example of poor design:

```c id="5jwej8"
send();
wait_ack();
retry();
timeout();
validate_packet();
```

inside the application.

This creates:

* duplicated code
* poor reusability
* difficult maintenance
* tight coupling

Correct architecture:

```c id="e98u9r"
lora_link_send_reliable();
```

Only the protocol layer should own reliability logic.

This is one of the most important design decisions in the project.

---

## Why Ping / Pong Instead of Only ACK

ACK confirms:

```text id="evn1xk"
delivery success
```

but does not demonstrate:

```text id="tf3n6m"
application-level response
```

Ping / Pong demonstrates:

```text id="bq6czi"
request
→ response
```

which is much closer to real systems such as:

* configuration commands
* remote status requests
* heartbeat validation
* sensor polling
* telemetry confirmation

This makes the project significantly stronger for real embedded applications.

---

## Node Addressing Strategy

The project uses:

```text id="o7k46g"
network_id
source_id
destination_id
```

## Why this matters

Two nodes should not communicate only because they are on the same frequency.

They must belong to the same network and have valid destination addressing.

Example:

```text id="z5owjt"
network_id = same group
source_id = who sent
destination_id = who should receive
```

This allows:

```text id="b7ynqj"
TX-A → RX-A
TX-B → RX-B
```

without interference.

This is critical for real embedded systems.

---

## Sleep → Standby → TX/RX

The SX127x requires:

```text id="pc9dzw"
Sleep
↓
LoRa mode enable
↓
Standby
↓
TX / RX
```

This is mandatory by the chip design.

It is not optional.

This initialization flow was intentionally implemented to follow the datasheet correctly.

---

## Validation Strategy

The project was validated in progressive stages:

1. SPI bring-up
2. SX127x register access
3. LoRa mode initialization
4. frequency configuration
5. TX basic packet transmission
6. TX/RX communication between two nodes
7. packet protocol layer
8. reliable delivery with ACK + retry
9. Ping / Pong bidirectional communication
10. protocol layer refactor (`lora_link`)

This reduced debugging complexity and isolated failures early.

---

## Design Goal

The objective is not only to make LoRa communication work.

The real goal is to provide:

```text id="xvf6xm"
a reusable and maintainable LoRa communication architecture
```

that can serve as the base for future telemetry systems and advanced embedded communication stacks.
