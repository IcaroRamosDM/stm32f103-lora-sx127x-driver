# STM32F103 + SX127x LoRa

[![Checks](https://github.com/IcaroRamosDM/stm32f103-lora-sx127x-driver/actions/workflows/checks.yml/badge.svg)](https://github.com/IcaroRamosDM/stm32f103-lora-sx127x-driver/actions/workflows/checks.yml)

Bare-metal STM32 firmware for a two-node LoRa link: SPI register access,
packet encoding, addressed DATA/ACK exchanges and PING/PONG responses.
It uses libopencm3 and keeps board setup, radio access and protocol logic separate.

## What is implemented

- SX127x configuration and polling-based transmit/receive.
- A six-byte packet header and up to 32 payload bytes.
- Capacity-checked encoding and exact-length decoding.
- Up to three DATA transmission attempts, with a 1000 ms ACK wait per attempt.
- Response matching by network, source, destination, packet identifier and type.
- Separate TX and RX firmware products, plus host protocol tests.

The default 915 MHz configuration requires a compatible radio and module.
SX1278 is specified for 137–525 MHz and must not be presented as a 915 MHz device.
See the [radio configuration](docs/sx127x.md) and the
[Semtech product table](https://www.semtech.com/products/wireless-rf/lora-connect).

## Build and run

[Toolchain and library setup](docs/build.md)

~~~sh
make tx
make rx
make test
make sanitize
~~~

TX and RX products are under build/TX/ and build/RX/.
Use make flash-tx or make flash-rx only when ready to program the corresponding board.

## Protocol and hardware

| Guide | Contents |
|---|---|
| [Architecture](docs/architecture.md) | Source layers and ownership |
| [Wiring](docs/wiring.md) | Board and radio connections |
| [Protocol](docs/protocol.md) | Packet layout, limits and API contracts |
| [Communication flow](docs/lora_flow.md) | DATA/ACK, PING/PONG and failure paths |
| [Radio](docs/sx127x.md) | Frequency, modulation and polling |

## License

[MIT](LICENSE).

## Author

**Icaro Ramos Rodrigues dos Santos**<br>
Electronic Engineer

Embedded Systems | Firmware | C | C++ | Python | Dart | Assembly | Hardware Design | PCB Design | STM32 | ARM Cortex-M | Linux | Windows | RISC-V | Verilog | SystemVerilog | UVM | Design Verification | Instrumentation | Signal Processing | Qt | FLTK

GitHub: [IcaroRamosDM](https://github.com/IcaroRamosDM)<br>
LinkedIn: [icaro-ramos-r](https://www.linkedin.com/in/icaro-ramos-r/)<br>
[icaroelt@gmail.com](mailto:icaroelt@gmail.com)
