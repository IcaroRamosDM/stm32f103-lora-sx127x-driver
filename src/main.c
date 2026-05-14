#include <stdint.h>

#include "app/app_lora_demo.h"

#include "bsp/clock.h"
#include "bsp/gpio.h"
#include "bsp/lora_gpio.h"
#include "bsp/systick.h"

#include "drivers/spi/spi_bus.h"
#include "drivers/sx127x/sx127x.h"
#include "drivers/sx127x/sx127x_registers.h"
#include "drivers/sx127x/sx127x_config.h"

#define MAIN_LOOP_DELAY_MS      2000U

int main(void) {

    clock_setup();
    systick_setup();
    gpio_setup();
    lora_gpio_setup();

    spi_bus_init();

    sx127x_init();
    sx127x_set_lora_mode();
    sx127x_set_frequency(SX127X_FREQUENCY_HZ);
    sx127x_configure_lora_defaults();

    app_lora_demo_init();

    while (1) {

        app_lora_demo_run();
    }

    return 0;
}
