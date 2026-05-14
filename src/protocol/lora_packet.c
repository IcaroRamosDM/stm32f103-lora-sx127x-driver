#include "protocol/lora_packet.h"

void lora_packet_init(
        lora_packet_t *packet
) {
    if (packet == 0U) {
        
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

uint8_t lora_packet_encode(
        const lora_packet_t *packet,
        uint8_t *buffer
) {
    if ((packet == 0U) || (buffer == 0U)) {

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

    return (uint8_t)(6U + packet->payload_length);
}

uint8_t lora_packet_decode(
        lora_packet_t *packet,
        const uint8_t *buffer,
        uint8_t buffer_length
) {
    if ((packet == 0U) || (buffer == 0U)) {

        return 0U;
    }

    if (buffer_length < 6U) {

        return 0U;
    }

    packet->network_id = buffer[0];
    packet->source_id = buffer[1];
    packet->destination_id = buffer[2];
    packet->packet_id = buffer[3];
    packet->packet_type = buffer[4];
    packet->payload_length = buffer[5];

    if (packet->payload_length > LORA_PACKET_MAX_PAYLOAD_SIZE) {

        return 0U;
    }

    for (uint8_t i = 0U; i < packet->payload_length; i++) {

        packet->payload[i] = buffer[6U + i];
    }

    return 1U;
}
