#include "drivers/sx127x/sx127x.h"
#include "drivers/sx127x/sx127x_registers.h"

#include "bsp/spi.h"
#include "bsp/lora_gpio.h"
#include "bsp/systick.h"

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

void sx127x_set_lora_mode(void) {

    sx127x_write_register(
            SX127X_REG_OP_MODE,
            SX127X_LONG_RANGE_MODE | SX127X_MODE_SLEEP
    );

    delay_ms(10U);

    sx127x_write_register(
            SX127X_REG_OP_MODE,
            SX127X_LONG_RANGE_MODE | SX127X_MODE_STDBY
    );

    delay_ms(10U);
}

void sx127x_set_frequency_915mhz(void) {
    /* FRF = (Frequency * 2^19) / 32MHz
       915MHz -> 0xE4C000 */

    sx127x_write_register(SX127X_REG_FRF_MSB, 0xE4U);
    sx127x_write_register(SX127X_REG_FRF_MID, 0xC0U);
    sx127x_write_register(SX127X_REG_FRF_LSB, 0x00U);
}
