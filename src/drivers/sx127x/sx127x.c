#include "drivers/sx127x/sx127x.h"
#include "drivers/sx127x/sx127x_registers.h"

#include "bsp/spi.h"
#include "bsp/lora_gpio.h"

void sx127x_init(void) {

    lora_reset_pulse();
}

uint8_t sx127x_read_register(uint8_t reg) {

    uint8_t value;
    
    spi_cs_select();

    spi_transfer_byte(reg & SX127X_READ_MASK);
    value = spi_transfer_byte(0x00U);

    spi_cs_deselect();

    return value;
}

void sx127x_write_register(uint8_t reg, uint8_t value) {

    spi_cs_select();

    spi_transfer_byte(reg | SX127X_WRITE_MASK);
    spi_transfer_byte(value);

    spi_cs_deselect();
}
