#pragma once

#include <stdint.h>

void sx127x_init(void);

uint8_t sx127x_read_register(uint8_t reg);
void sx127x_write_register(uint8_t reg, uint8_t value);

void sx127x_set_lora_mode(void);
void sx127x_set_frequency_915mhz(void);

void sx127x_send_packet(
        const uint8_t *data,
        uint8_t length
);
