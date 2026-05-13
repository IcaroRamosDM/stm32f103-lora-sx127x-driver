#include <stdint.h>

#include "bsp/clock.h"
#include "bsp/gpio.h"
#include "bsp/lora_gpio.h"
#include "bsp/systick.h"

#include "drivers/spi/spi_bus.h"
#include "drivers/sx127x/sx127x.h"
#include "drivers/sx127x/sx127x_registers.h"

#define MAIN_LOOP_DELAY_MS      2000U

int main(void) {

    static const uint8_t message[] = "HELLO";

    clock_setup();
    systick_setup();
    gpio_setup();
    lora_gpio_setup();

    spi_bus_init();
    sx127x_init();

    sx127x_set_lora_mode();
    sx127x_set_frequency_915mhz();

    while (1) {

        sx127x_send_packet(
                message,
                sizeof(message) - 1U
        );

        gpio_led_toggle();
        delay_ms(MAIN_LOOP_DELAY_MS);
    }

    return 0;
}
