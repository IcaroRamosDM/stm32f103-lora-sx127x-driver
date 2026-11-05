#include "protocol/lora_packet.h"

#include <stddef.h>
#include <string.h>

static uint8_t packet_type_is_valid(uint8_t packet_type) {
    return packet_type == LORA_PACKET_TYPE_DATA || packet_type == LORA_PACKET_TYPE_ACK ||
           packet_type == LORA_PACKET_TYPE_PING || packet_type == LORA_PACKET_TYPE_PONG;
}

void lora_packet_init(lora_packet_t *packet) {
    if (packet == NULL) {

        return;
    }

    packet->network_id = LORA_NETWORK_ID;
    packet->source_id = 0U;
    packet->destination_id = 0U;
    packet->packet_id = 0U;
    packet->packet_type = LORA_PACKET_TYPE_DATA;
    packet->payload_length = 0U;

    for (uint8_t i = 0U; i < LORA_PACKET_MAX_PAYLOAD_SIZE; i++) {

        packet->payload[i] = 0U;
    }
}

uint8_t lora_packet_encode(const lora_packet_t *packet, uint8_t *buffer, uint8_t buffer_capacity) {
    if (packet == NULL || buffer == NULL || packet->payload_length > LORA_PACKET_MAX_PAYLOAD_SIZE ||
        !packet_type_is_valid(packet->packet_type)) {

        return 0U;
    }

    const uint8_t encoded_length = (uint8_t)(LORA_PACKET_HEADER_SIZE + packet->payload_length);

    if (buffer_capacity < encoded_length) {

        return 0U;
    }

    buffer[0] = packet->network_id;
    buffer[1] = packet->source_id;
    buffer[2] = packet->destination_id;
    buffer[3] = packet->packet_id;
    buffer[4] = packet->packet_type;
    buffer[5] = packet->payload_length;

    for (uint8_t i = 0U; i < packet->payload_length; i++) {

        buffer[6U + i] = packet->payload[i];
    }

    return encoded_length;
}

uint8_t lora_packet_decode(lora_packet_t *packet, const uint8_t *buffer, uint8_t buffer_length) {
    if ((packet == NULL) || (buffer == NULL)) {

        return 0U;
    }

    if (buffer_length < LORA_PACKET_HEADER_SIZE) {

        return 0U;
    }

    const uint8_t payload_length = buffer[5];
    if (payload_length > LORA_PACKET_MAX_PAYLOAD_SIZE ||
        buffer_length != LORA_PACKET_HEADER_SIZE + payload_length ||
        !packet_type_is_valid(buffer[4])) {

        return 0U;
    }

    /* Validate the complete frame before changing the caller's packet. */
    lora_packet_t decoded = {.network_id = buffer[0],
                             .source_id = buffer[1],
                             .destination_id = buffer[2],
                             .packet_id = buffer[3],
                             .packet_type = buffer[4],
                             .payload_length = payload_length};
    memcpy(decoded.payload, buffer + LORA_PACKET_HEADER_SIZE, payload_length);
    *packet = decoded;

    return 1U;
}
