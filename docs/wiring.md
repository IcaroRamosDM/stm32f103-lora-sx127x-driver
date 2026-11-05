# Wiring

| STM32F103 | SX127x module |
|---|---|
| PA5 | SCK |
| PA6 | MISO |
| PA7 | MOSI |
| PA4 | NSS |
| PB0 | RESET |
| PB1 | DIO0, reserved for a future interrupt path |
| 3.3 V | VCC |
| GND | GND |

Use the radio module's specified supply and a common ground. Do not feed a
3.3 V-only radio module from 5 V. Connect the antenna intended for its RF band.
The default frequency is 915 MHz; see sx127x.md before choosing a chip/module.

Two nodes need matching modulation, frequency, sync word and network settings.
Programming uses ST-Link. Host tests do not require radio hardware.
