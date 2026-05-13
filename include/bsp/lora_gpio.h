#pragma once

#include <stdint.h>

void lora_gpio_setup(void);

void lora_reset_assert(void);
void lora_reset_release(void);
void lora_reset_pulse(void);

uint8_t lora_dio0_read(void);
