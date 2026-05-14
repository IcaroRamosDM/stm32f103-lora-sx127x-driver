#include "drivers/sx127x/sx127x.h"
#include "drivers/sx127x/sx127x_registers.h"
#include "drivers/sx127x/sx127x_config.h"

#include "bsp/spi.h"
#include "bsp/lora_gpio.h"
#include "bsp/systick.h"

#define SX127X_PA_BOOST                  0x80U
#define SX127X_PA_OUTPUT_POWER_MASK      0x0FU

#define SX127X_MODEM_CONFIG_1_BW_SHIFT   4U
#define SX127X_MODEM_CONFIG_1_CR_SHIFT   1U

#define SX127X_MODEM_CONFIG_2_SF_SHIFT   4U
#define SX127X_MODEM_CONFIG_2_CRC_ON     0x04U

#define SX127X_MODEM_CONFIG_3_AGC_AUTO   0x04U

#define SX127X_MIN_TX_POWER_DBM          2U
#define SX127X_MAX_TX_POWER_DBM          17U

#define SX127X_TX_DONE_TIMEOUT_MS         1000U

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

void sx127x_configure_lora_defaults(void) {

    const uint8_t pa_config =
        SX127X_PA_BOOST |
        ((SX127X_TX_POWER_DBM - SX127X_MIN_TX_POWER_DBM) &
         SX127X_PA_OUTPUT_POWER_MASK);

    const uint8_t modem_config_1 =
        (uint8_t)((SX127X_LORA_BANDWIDTH << SX127X_MODEM_CONFIG_1_BW_SHIFT) |
                  (SX127X_LORA_CODING_RATE << SX127X_MODEM_CONFIG_1_CR_SHIFT));

    const uint8_t modem_config_2 =
        (uint8_t)((SX127X_LORA_SPREADING_FACTOR << SX127X_MODEM_CONFIG_2_SF_SHIFT) |
                  (SX127X_LORA_CRC_ENABLED ? SX127X_MODEM_CONFIG_2_CRC_ON : 0U));

    sx127x_write_register(SX127X_REG_PA_CONFIG, pa_config);
    sx127x_write_register(SX127X_REG_MODEM_CONFIG_1, modem_config_1);
    sx127x_write_register(SX127X_REG_MODEM_CONFIG_2, modem_config_2);
    sx127x_write_register(SX127X_REG_MODEM_CONFIG_3, SX127X_MODEM_CONFIG_3_AGC_AUTO);

    sx127x_write_register(
            SX127X_REG_PREAMBLE_MSB,
            (uint8_t)(SX127X_PREAMBLE_LENGTH >> 8U)
    );

    sx127x_write_register(
            SX127X_REG_PREAMBLE_LSB,
            (uint8_t)(SX127X_PREAMBLE_LENGTH)
    );

    sx127x_write_register(SX127X_REG_SYNC_WORD, SX127X_SYNC_WORD_PRIVATE);
    sx127x_write_register(SX127X_REG_IRQ_FLAGS, SX127X_IRQ_CLEAR_ALL);
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

void sx127x_set_frequency(uint32_t frequency_hz) {

    const uint64_t frf =
        ((uint64_t)frequency_hz * SX127X_FRF_SCALE) /
        SX127X_CRYSTAL_FREQUENCY_HZ;

    sx127x_write_register(SX127X_REG_FRF_MSB, (uint8_t)(frf >> 16U));
    sx127x_write_register(SX127X_REG_FRF_MID, (uint8_t)(frf >> 8U));
    sx127x_write_register(SX127X_REG_FRF_LSB, (uint8_t)(frf));
}

void sx127x_send_packet(
        const uint8_t *data,
        uint8_t length
) {
    uint16_t timeout_ms = 0U;

    if ((data == 0U) || (length == 0U)) {

        return;
    }

    sx127x_write_register(
            SX127X_REG_OP_MODE,
            SX127X_LONG_RANGE_MODE | SX127X_MODE_STDBY
    );

    sx127x_write_register(
            SX127X_REG_IRQ_FLAGS,
            SX127X_IRQ_CLEAR_ALL
    );

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

    while (((sx127x_read_register(SX127X_REG_IRQ_FLAGS) & SX127X_IRQ_TX_DONE) == 0U) &&
            (timeout_ms < SX127X_TX_DONE_TIMEOUT_MS)) {

        delay_ms(1U);
        timeout_ms++;
    }

    sx127x_write_register(
            SX127X_REG_IRQ_FLAGS,
            SX127X_IRQ_CLEAR_ALL
    );

    sx127x_write_register(
            SX127X_REG_OP_MODE,
            SX127X_LONG_RANGE_MODE | SX127X_MODE_STDBY
    );
}

void sx127x_start_receive(void) {

    sx127x_write_register(
            SX127X_REG_FIFO_RX_BASE_ADDR,
            0x00U
    );

    sx127x_write_register(
            SX127X_REG_FIFO_ADDR_PTR,
            0x00U
    );

    sx127x_write_register(
            SX127X_REG_IRQ_FLAGS,
            SX127X_IRQ_CLEAR_ALL
    );

    sx127x_write_register(
            SX127X_REG_OP_MODE,
            SX127X_LONG_RANGE_MODE | SX127X_MODE_RX_CONTINUOUS
    );
}

uint8_t sx127x_receive_packet(
        uint8_t *buffer,
        uint8_t max_length
) {
    uint8_t irq_flags;
    uint8_t length;
    uint8_t current_addr;

    if ((buffer == 0U) || (max_length == 0U)) {

        return 0U;
    }

    irq_flags = sx127x_read_register(SX127X_REG_IRQ_FLAGS);

    if ((irq_flags & SX127X_IRQ_RX_DONE) == 0U) {

        return 0U;
    }

    length = sx127x_read_register(SX127X_REG_RX_NB_BYTES);
    current_addr = sx127x_read_register(SX127X_REG_FIFO_RX_CURRENT_ADDR);

    if (length > max_length) {

        length = max_length;
    }

    sx127x_write_register(
            SX127X_REG_FIFO_ADDR_PTR,
            current_addr
    );

    for (uint8_t i = 0U; i < length; i++) {

        buffer[i] = sx127x_read_register(SX127X_REG_FIFO);
    }

    sx127x_write_register(
            SX127X_REG_IRQ_FLAGS,
            SX127X_IRQ_CLEAR_ALL
    );

    return length;
}
