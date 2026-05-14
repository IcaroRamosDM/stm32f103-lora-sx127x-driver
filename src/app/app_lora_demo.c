#include "app/app_lora_demo.h"
#include "app/app_lora_demo_config.h"

#include "bsp/gpio.h"
#include "bsp/systick.h"

#include "drivers/sx127x/sx127x.h"

#include "protocol/lora_packet.h"

#include <stdint.h>

#define APP_LORA_TX_DELAY_MS        2000U
#define APP_LORA_RX_BUFFER_SIZE     64U

#define APP_ACK_TIMEOUT_MS          1000U
#define APP_MAX_RETRY_COUNT         3U

#define APP_ACK_BLINK_MS            150U
#define APP_RX_BLINK_MS             200U

#if (APP_NODE_IS_TX != 0U)

static uint8_t app_packet_id = 0U;

static uint8_t app_wait_for_ack(
  uint8_t expected_packet_id
);

static void app_lora_demo_tx_run(void);

#endif

#if (APP_NODE_IS_RX != 0U)

static void app_send_ack(
  uint8_t destination_id,
  uint8_t packet_id
);

static void app_lora_demo_rx_run(void);

#endif

void app_lora_demo_init(void)
{
#if (APP_NODE_IS_RX != 0U)
  sx127x_start_receive();
#endif
}

void app_lora_demo_run(void)
{
#if (APP_NODE_IS_TX != 0U)
  app_lora_demo_tx_run();
#elif (APP_NODE_IS_RX != 0U)
  app_lora_demo_rx_run();
#endif
}

#if (APP_NODE_IS_TX != 0U)

static uint8_t app_wait_for_ack(
  uint8_t expected_packet_id
)
{
  uint8_t rx_buffer[APP_LORA_RX_BUFFER_SIZE];
  uint16_t timeout = 0U;

  while (timeout < APP_ACK_TIMEOUT_MS)
  {
    uint8_t received_length;
    lora_packet_t packet;

    received_length = sx127x_receive_packet(
      rx_buffer,
      APP_LORA_RX_BUFFER_SIZE
    );

    if (received_length > 0U)
    {
      if (lora_packet_decode(
            &packet,
            rx_buffer,
            received_length
          ) != 0U)
      {
        if ((packet.packet_type == LORA_PACKET_TYPE_ACK) &&
            (packet.packet_id == expected_packet_id) &&
            (packet.destination_id == LORA_NODE_TX_ID))
        {
          gpio_led_set(true);
          delay_ms(APP_ACK_BLINK_MS);
          gpio_led_set(false);

          return 1U;
        }
      }

      sx127x_start_receive();
    }

    delay_ms(1U);
    timeout++;
  }

  return 0U;
}

static void app_lora_demo_tx_run(void)
{
  lora_packet_t packet;
  uint8_t tx_buffer[64U];
  uint8_t encoded_length;
  uint8_t success = 0U;

  lora_packet_init(&packet);

  packet.source_id = LORA_NODE_TX_ID;
  packet.destination_id = LORA_NODE_RX_ID;
  packet.packet_id = app_packet_id;
  packet.packet_type = LORA_PACKET_TYPE_DATA;
  packet.payload_length = 5U;

  packet.payload[0] = 'H';
  packet.payload[1] = 'E';
  packet.payload[2] = 'L';
  packet.payload[3] = 'L';
  packet.payload[4] = 'O';

  encoded_length = lora_packet_encode(
    &packet,
    tx_buffer
  );

  for (uint8_t retry = 0U;
       retry < APP_MAX_RETRY_COUNT;
       retry++)
  {
    sx127x_send_packet(
      tx_buffer,
      encoded_length
    );

    sx127x_start_receive();

    if (app_wait_for_ack(app_packet_id) != 0U)
    {
      success = 1U;
      break;
    }
  }

  if (success != 0U)
  {
    app_packet_id++;
  }

  delay_ms(APP_LORA_TX_DELAY_MS);
}

#endif

#if (APP_NODE_IS_RX != 0U)

static void app_send_ack(
  uint8_t destination_id,
  uint8_t packet_id
)
{
  lora_packet_t ack_packet;
  uint8_t tx_buffer[32U];
  uint8_t encoded_length;

  lora_packet_init(&ack_packet);

  ack_packet.source_id = LORA_NODE_RX_ID;
  ack_packet.destination_id = destination_id;
  ack_packet.packet_id = packet_id;
  ack_packet.packet_type = LORA_PACKET_TYPE_ACK;
  ack_packet.payload_length = 0U;

  encoded_length = lora_packet_encode(
    &ack_packet,
    tx_buffer
  );

  sx127x_send_packet(
    tx_buffer,
    encoded_length
  );
}

static void app_lora_demo_rx_run(void)
{
  uint8_t rx_buffer[APP_LORA_RX_BUFFER_SIZE];
  uint8_t received_length;
  lora_packet_t packet;

  received_length = sx127x_receive_packet(
    rx_buffer,
    APP_LORA_RX_BUFFER_SIZE
  );

  if (received_length == 0U)
  {
    return;
  }

  if (lora_packet_decode(
        &packet,
        rx_buffer,
        received_length
      ) == 0U)
  {
    sx127x_start_receive();
    return;
  }

  if ((packet.network_id != LORA_NETWORK_ID) ||
      (packet.destination_id != LORA_NODE_RX_ID))
  {
    sx127x_start_receive();
    return;
  }

  if (packet.packet_type == LORA_PACKET_TYPE_DATA)
  {
    gpio_led_set(true);
    delay_ms(APP_RX_BLINK_MS);
    gpio_led_set(false);

    app_send_ack(
      packet.source_id,
      packet.packet_id
    );
  }

  sx127x_start_receive();
}

#endif
