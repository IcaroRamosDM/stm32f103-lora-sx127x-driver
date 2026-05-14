#include "app/app_lora_demo.h"

#include "app/app_lora_demo_config.h"

#include "bsp/gpio.h"
#include "bsp/systick.h"

#include "protocol/lora_link.h"
#include "protocol/lora_packet.h"

#include <stdint.h>

#define APP_LORA_TX_DELAY_MS  2000U

void app_lora_demo_init(void)
{
#if (APP_NODE_IS_RX != 0U)
  lora_link_init();
#endif
}

void app_lora_demo_run(void)
{
#if (APP_NODE_IS_TX != 0U)

  (void) lora_link_send_ping(
    LORA_NODE_RX_ID
  );

  delay_ms(APP_LORA_TX_DELAY_MS);

#elif (APP_NODE_IS_RX != 0U)

  (void) lora_link_receive_ping_and_respond();

#endif
}
