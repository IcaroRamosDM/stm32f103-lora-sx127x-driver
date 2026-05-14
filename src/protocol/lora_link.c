#include "protocol/lora_link.h"

#include "bsp/gpio.h"
#include "bsp/systick.h"

#include "drivers/sx127x/sx127x.h"

#include "protocol/lora_packet.h"

#define LORA_LINK_RX_BUFFER_SIZE   64U
#define LORA_LINK_ACK_TIMEOUT_MS   1000U
#define LORA_LINK_MAX_RETRIES      3U

#define LORA_LINK_ACK_BLINK_MS     150U
#define LORA_LINK_RX_BLINK_MS      200U

static uint8_t lora_link_packet_id = 0U;

static uint8_t lora_link_wait_for_ack(uint8_t expected_packet_id);
static void lora_link_send_ack(uint8_t destination_id, uint8_t packet_id);

void lora_link_init(void)
{
  sx127x_start_receive();
}

uint8_t lora_link_send_reliable(
  uint8_t destination_id,
  const uint8_t *payload,
  uint8_t payload_length
)
{
  lora_packet_t packet;
  uint8_t tx_buffer[LORA_LINK_RX_BUFFER_SIZE];
  uint8_t encoded_length;
  uint8_t success = 0U;

  if ((payload == 0U) || (payload_length == 0U))
  {
    return LORA_LINK_STATUS_FAILED;
  }

  lora_packet_init(&packet);

  packet.source_id = LORA_NODE_TX_ID;
  packet.destination_id = destination_id;
  packet.packet_id = lora_link_packet_id;
  packet.packet_type = LORA_PACKET_TYPE_DATA;
  packet.payload_length = payload_length;

  for (uint8_t i = 0U; i < payload_length; i++)
  {
    packet.payload[i] = payload[i];
  }

  encoded_length = lora_packet_encode(&packet, tx_buffer);

  for (uint8_t retry = 0U; retry < LORA_LINK_MAX_RETRIES; retry++)
  {
    sx127x_send_packet(tx_buffer, encoded_length);
    sx127x_start_receive();

    if (lora_link_wait_for_ack(lora_link_packet_id) != 0U)
    {
      success = 1U;
      break;
    }
  }

  if (success != 0U)
  {
    lora_link_packet_id++;
    return LORA_LINK_STATUS_OK;
  }

  return LORA_LINK_STATUS_FAILED;
}

uint8_t lora_link_receive_and_ack(void)
{
  uint8_t rx_buffer[LORA_LINK_RX_BUFFER_SIZE];
  uint8_t received_length;
  lora_packet_t packet;

  received_length = sx127x_receive_packet(
    rx_buffer,
    LORA_LINK_RX_BUFFER_SIZE
  );

  if (received_length == 0U)
  {
    return 0U;
  }

  if (lora_packet_decode(&packet, rx_buffer, received_length) == 0U)
  {
    sx127x_start_receive();
    return 0U;
  }

  if ((packet.network_id != LORA_NETWORK_ID) ||
      (packet.destination_id != LORA_NODE_RX_ID))
  {
    sx127x_start_receive();
    return 0U;
  }

  if (packet.packet_type == LORA_PACKET_TYPE_DATA)
  {
    gpio_led_set(true);
    delay_ms(LORA_LINK_RX_BLINK_MS);
    gpio_led_set(false);

    lora_link_send_ack(packet.source_id, packet.packet_id);
    sx127x_start_receive();

    return 1U;
  }

  sx127x_start_receive();
  return 0U;
}

static uint8_t lora_link_wait_for_ack(uint8_t expected_packet_id)
{
  uint8_t rx_buffer[LORA_LINK_RX_BUFFER_SIZE];
  uint16_t timeout = 0U;

  while (timeout < LORA_LINK_ACK_TIMEOUT_MS)
  {
    uint8_t received_length;
    lora_packet_t packet;

    received_length = sx127x_receive_packet(
      rx_buffer,
      LORA_LINK_RX_BUFFER_SIZE
    );

    if (received_length > 0U)
    {
      if (lora_packet_decode(&packet, rx_buffer, received_length) != 0U)
      {
        if ((packet.packet_type == LORA_PACKET_TYPE_ACK) &&
            (packet.packet_id == expected_packet_id) &&
            (packet.destination_id == LORA_NODE_TX_ID))
        {
          gpio_led_set(true);
          delay_ms(LORA_LINK_ACK_BLINK_MS);
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

static void lora_link_send_ack(uint8_t destination_id, uint8_t packet_id)
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

  encoded_length = lora_packet_encode(&ack_packet, tx_buffer);
  sx127x_send_packet(tx_buffer, encoded_length);
}
