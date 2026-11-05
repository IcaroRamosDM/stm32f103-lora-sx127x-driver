#include "bsp/lora_gpio.h"

#include <libopencm3/stm32/gpio.h>
#include "bsp/systick.h"

#define LORA_RST_PORT GPIOB
#define LORA_RST_PIN GPIO0

#define LORA_DIO0_PORT GPIOB
#define LORA_DIO0_PIN GPIO1

#define LORA_RESET_DELAY_MS 10U

void lora_gpio_setup(void) {

    gpio_set_mode(LORA_RST_PORT, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, LORA_RST_PIN);

    gpio_set_mode(LORA_DIO0_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, LORA_DIO0_PIN);

    lora_reset_release();
}

void lora_reset_assert(void) {

    gpio_clear(LORA_RST_PORT, LORA_RST_PIN);
}

void lora_reset_release(void) {

    gpio_set(LORA_RST_PORT, LORA_RST_PIN);
}

void lora_reset_pulse(void) {

    lora_reset_assert();
    delay_ms(LORA_RESET_DELAY_MS);

    lora_reset_release();
    delay_ms(LORA_RESET_DELAY_MS);
}

uint8_t lora_dio0_read(void) {

    return (gpio_get(LORA_DIO0_PORT, LORA_DIO0_PIN) != 0U);
}
