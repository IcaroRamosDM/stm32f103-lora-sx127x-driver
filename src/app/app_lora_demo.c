#include "app/app_lora_demo.h"

#include "app/app_lora_demo_config.h"

#include "bsp/gpio.h"
#include "bsp/systick.h"

#include "drivers/sx127x/sx127x.h"

#include <stdint.h>

#define APP_LORA_TX_DELAY_MS        2000U
#define APP_LORA_RX_BUFFER_SIZE     32U
#define APP_LORA_RX_BLINK_MS        200U

#if (APP_NODE_IS_TX != 0U)
static const uint8_t app_lora_tx_message[] = "HELLO";
static void app_lora_demo_tx_run(void);
#endif

#if (APP_NODE_IS_RX != 0U)
static void app_lora_demo_rx_run(void);
#endif

void app_lora_demo_init(void) {
#if (APP_NODE_IS_RX != 0U)
    sx127x_start_receive();
#endif
}

void app_lora_demo_run(void) {
#if (APP_NODE_IS_TX != 0U)
    app_lora_demo_tx_run();
#elif (APP_NODE_IS_RX != 0U)
    app_lora_demo_rx_run();
#endif
}

#if (APP_NODE_IS_TX != 0U)
static void app_lora_demo_tx_run(void) {

    sx127x_send_packet(
            app_lora_tx_message,
            sizeof(app_lora_tx_message) - 1U
    );

    gpio_led_toggle();
    delay_ms(APP_LORA_TX_DELAY_MS);
}
#endif

#if (APP_NODE_IS_RX != 0U)
static void app_lora_demo_rx_run(void) {

    uint8_t rx_buffer[APP_LORA_RX_BUFFER_SIZE];
    uint8_t received_length;

    received_length = sx127x_receive_packet(
            rx_buffer,
            APP_LORA_RX_BUFFER_SIZE
    );

    if (received_length > 0U) {

        gpio_led_set(true);
        delay_ms(APP_LORA_RX_BLINK_MS);
        gpio_led_set(false);
        delay_ms(APP_LORA_RX_BLINK_MS);

        sx127x_start_receive();
    }
}
#endif
