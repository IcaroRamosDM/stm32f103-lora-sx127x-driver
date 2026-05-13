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

void sx127x_send_packet(
        const uint8_t *data,
        uint8_t length
) {
    if ((data == 0U) || (length == 0U)) {

        return;
    }

    sx127x_write_register(
            SX127X_REG_FIFO_TX_BASE_ADDR,
            0x00U
    );

    sx127x_write_register(
            SX127X_REG_FIFO_ADDR_PTR,
            0x00U
    );

    for (uint8_t i = 0U; i < length; i++) {

        sx127x_write_register(
                SX127X_REG_FIFO,
                data[i]
        );
    }

    sx127x_write_register(
            SX127X_REG_PAYLOAD_LENGTH,
            length
    );

    sx127x_write_register(
            SX127X_REG_OP_MODE,
            SX127X_LONG_RANGE_MODE | SX127X_MODE_TX
    );

    delay_ms(100U);

    sx127x_write_register(
            SX127X_REG_OP_MODE,
            SX127X_LONG_RANGE_MODE | SX127X_MODE_STDBY
    );
}
