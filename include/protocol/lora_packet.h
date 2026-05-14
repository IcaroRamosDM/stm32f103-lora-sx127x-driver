#pragma once

#include <stdint.h>

#define LORA_PACKET_MAX_PAYLOAD_SIZE        32U

#define LORA_PACKET_TYPE_DATA               1U
#define LORA_PACKET_TYPE_ACK                2U
#define LORA_PACKET_TYPE_PING               3U
#define LORA_PACKET_TYPE_PONG               4U

#define LORA_NETWORK_ID                     0xA1U

#define LORA_NODE_TX_ID                     0x01U
#define LORA_NODE_RX_ID                     0x02U

typedef struct {
    uint8_t network_id;
    uint8_t source_id;
    uint8_t destination_id;
    uint8_t packet_id;
    uint8_t packet_type;
    uint8_t payload_length;
    uint8_t payload[LORA_PACKET_MAX_PAYLOAD_SIZE];

} lora_packet_t;

void lora_packet_init(
        lora_packet_t *packet
);

uint8_t lora_packet_encode(
        const lora_packet_t *packet,
        uint8_t *buffer
);

uint8_t lora_packet_decode(
        lora_packet_t *packet,
        const uint8_t *buffer,
        uint8_t buffer_length
);
