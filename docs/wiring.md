# Wiring Guide

## Overview

This project uses two STM32F103C8T6 (Blue Pill) boards and two SX1276 / SX1278 LoRa modules for bidirectional communication testing.

The goal is to validate:

* TX / RX communication
* ACK + retry
* Ping / Pong request-response flow
* reliable packet delivery
* node addressing

Correct wiring is critical.

Most LoRa communication failures are caused by hardware mistakes, not firmware.

This document defines the exact recommended connections and common mistakes to avoid.

---

## Required Hardware

### MCU

* 2x STM32F103C8T6 (Blue Pill)

### Radio Modules

* 2x SX1276 / SX1278 LoRa modules

Examples:

```text id="w1"
XL1276
XL1278
SX1276/1278
Ra-02 compatible modules
```

Modules with:

```text id="w2"
MISO
MOSI
SCK
NSS
RST
DIO0
DIO1
DIO2
DIO3
DIO4
DIO5
```

are fully compatible with this project.

---

## Programming Interface

* ST-Link V2

Used for:

* flashing firmware
* debugging
* validation

---

## Optional Hardware

* jumper wires
* breadboard
* USB-TTL (future debug expansion)
* external power source (future low-power validation)

---

## Important Electrical Rules

## Use Only 3.3V

The SX127x is a:

```text id="w3"
3.3V device
```

Never use:

```text id="w4"
5V
```

for:

* VCC
* SPI pins
* RESET
* DIO pins

Using 5V may permanently damage the module.

This is one of the most common mistakes.

---

## Antenna Is Mandatory

Always connect the antenna before powering the module.

Never power the radio without antenna attached.

This can damage the RF stage.

Do not skip this.

---

## Shared Ground Is Mandatory

Both the STM32 and the LoRa module must share:

```text id="w5"
GND
```

Without common ground, SPI communication becomes unreliable or completely fails.

This is another common failure source.

---

# TX Node Wiring

## Blue Pill → SX127x

| STM32 | Function      | SX127x |
| ----- | ------------- | ------ |
| PA5   | SPI1 SCK      | SCK    |
| PA6   | SPI1 MISO     | MISO   |
| PA7   | SPI1 MOSI     | MOSI   |
| PA4   | Manual NSS    | NSS    |
| PB0   | RESET control | RST    |
| PB1   | DIO0 IRQ line | DIO0   |
| 3V3   | Power         | VCC    |
| GND   | Ground        | GND    |

This same wiring is used for both TX and RX nodes.

Only the firmware role changes.

Hardware remains identical.

---

# RX Node Wiring

Exactly the same as TX.

The node role is defined by:

```bash id="w6"
make flash-tx
make flash-rx
```

and not by hardware changes.

This simplifies testing significantly.

---

# Why DIO0 Matters

DIO0 is the main radio event pin.

It signals events such as:

* RX_DONE
* TX_DONE
* packet reception

This project currently uses:

```text id="w7"
polling
```

instead of:

```text id="w8"
interrupts
```

but DIO0 is still wired intentionally.


---

# Why RESET Matters

RST allows:

* hardware reset
* deterministic startup
* recovery from invalid radio states

Example:

```text id="w10"
startup reset sequence
```

This improves initialization reliability.

The project intentionally controls RESET by GPIO instead of relying only on power-up behavior.

---

# Why Manual NSS Matters

This project uses:

```text id="w11"
manual chip select
```

instead of hardware-managed NSS.

This provides:

* better timing control
* predictable transactions
* reusable SPI architecture
* easier debugging

This is a deliberate design decision.

It is also useful for future peripherals.

---

# ST-Link Wiring

## Blue Pill Programming

| ST-Link | STM32 |
| ------- | ----- |
| SWDIO   | SWDIO |
| SWCLK   | SWCLK |
| GND     | GND   |
| 3V3     | 3V3   |

Optional:

| ST-Link | STM32 |
| ------- | ----- |
| NRST    | NRST  |

Used for:

```bash id="w12"
make flash-tx
make flash-rx
```

and:

```bash id="w13"
make reflash-tx
make reflash-rx
```

---

# Common Mistakes

## Wrong Voltage

Using:

```text id="w14"
5V
```

instead of:

```text id="w15"
3.3V
```

This can destroy the radio.

---

## No Antenna

Powering the module without antenna connected.

This can damage the RF power stage.

---

## Missing Ground

Forgetting shared GND.

This causes unstable SPI communication.

---

## Wrong SPI Pins

Using:

```text id="w16"
SPI2 pins
```

instead of:

```text id="w17"
SPI1 pins
```

This project uses:

```text id="w18"
SPI1 only
```

---

## Swapped MISO / MOSI

Very common during bring-up.

Always verify:

```text id="w19"
PA6 → MISO
PA7 → MOSI
```

---

## Wrong Frequency Module Assumption

Some modules are marked:

```text id="w20"
433 MHz
868 MHz
915 MHz
```

This project uses:

```text id="w21"
915 MHz
```

Always verify the hardware label.

Using the wrong frequency band may cause communication failure.

---

# Initial Validation Strategy

Before testing full communication:

validate in stages:

1. SPI register access
2. SX127x version register
3. LoRa mode enable
4. frequency setup
5. TX basic packet
6. RX packet reception
7. ACK validation
8. Ping / Pong validation

Do not debug everything at once.

Progressive validation reduces complexity significantly.

This was the strategy used in the project.

---

# Design Goal

The objective is not only:

```text id="w22"
connect wires and make it work
```

The real goal is:

```text id="w23"
build a reliable and repeatable hardware validation process
```

that supports future embedded communication systems.
