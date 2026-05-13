#include "bsp/clock.h"
#include "bsp/gpio.h"
#include "bsp/lora_gpio.h"
#include "bsp/systick.h"
#include "drivers/spi/spi_bus.h"

#define MAIN_LOOP_DELAY_MS      500U

int main(void) {

    clock_setup();
    systick_setup();
    gpio_setup();
    lora_gpio_setup();

    spi_bus_init();

    lora_reset_pulse();

    while (1) {

        gpio_led_toggle();
        delay_ms(MAIN_LOOP_DELAY_MS);
    }

    return 0;
}
