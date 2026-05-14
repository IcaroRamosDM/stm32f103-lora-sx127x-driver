#include "app/app_lora_demo.h"

#include "app/app_lora_demo_config.h"

#include "bsp/gpio.h"
#include "bsp/systick.h"

#include "protocol/lora_link.h"
#include "protocol/lora_packet.h"

#include <stdint.h>

#define APP_LORA_TX_DELAY_MS  2000U

#if (APP_NODE_IS_TX != 0U)
static const uint8_t app_lora_tx_message[] = "HELLO";
#endif

void app_lora_demo_init(void)
{
#if (APP_NODE_IS_RX != 0U)
  lora_link_init();
#endif
}

void app_lora_demo_run(void)
{
#if (APP_NODE_IS_TX != 0U)

  (void)lora_link_send_reliable(
    LORA_NODE_RX_ID,
    app_lora_tx_message,
    sizeof(app_lora_tx_message) - 1U
  );

  delay_ms(APP_LORA_TX_DELAY_MS);

#elif (APP_NODE_IS_RX != 0U)

  (void)lora_link_receive_and_ack();

#endif
}
