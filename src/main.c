#include <stdint.h>

#include "bsp/clock.h"
#include "bsp/gpio.h"
#include "bsp/lora_gpio.h"
#include "bsp/systick.h"

#include "drivers/spi/spi_bus.h"
#include "drivers/sx127x/sx127x.h"
#include "drivers/sx127x/sx127x_registers.h"

#define MAIN_LOOP_DELAY_MS      500U

int main(void) {

    uint8_t version;

    clock_setup();
    systick_setup();
    gpio_setup();
    lora_gpio_setup();

    spi_bus_init();
    sx127x_init();

    version = sx127x_read_register(SX127X_REG_VERSION);

    while (1) {

        for (uint8_t i = 0U; i < version; i++) {

            gpio_led_toggle();
            delay_ms(300U);
            gpio_led_toggle();
            delay_ms(300U);
        }

        delay_ms(1000U);
    }

    return 0;
}
