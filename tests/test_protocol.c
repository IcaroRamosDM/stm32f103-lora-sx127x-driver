#include "protocol/lora_link.h"
#include "protocol/lora_packet.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) check((condition), #condition, __LINE__)
#define RADIO_QUEUE_CAPACITY 16U

typedef struct {
    uint8_t bytes[LORA_PACKET_MAX_ENCODED_SIZE];
    uint8_t length;
} radio_frame;

static radio_frame incoming[RADIO_QUEUE_CAPACITY];
static radio_frame outgoing[RADIO_QUEUE_CAPACITY];
static unsigned incoming_count;
static unsigned incoming_position;
static unsigned outgoing_count;
static unsigned checks;

static void check(bool condition, const char *expression, int line) {
    checks++;
    if (!condition) {
        fprintf(stderr, "test_protocol.c:%d: %s\n", line, expression);
        exit(EXIT_FAILURE);
    }
}

void gpio_led_set(bool enabled) {
    (void)enabled;
}

void delay_ms(uint32_t milliseconds) {
    (void)milliseconds;
}

void sx127x_start_receive(void) {
}

void sx127x_send_packet(const uint8_t *bytes, uint8_t length) {
    CHECK(outgoing_count < RADIO_QUEUE_CAPACITY);
    CHECK(length <= LORA_PACKET_MAX_ENCODED_SIZE);
    outgoing[outgoing_count].length = length;
    memcpy(outgoing[outgoing_count].bytes, bytes, length);
    outgoing_count++;
}

uint8_t sx127x_receive_packet(uint8_t *bytes, uint8_t capacity) {
    if (incoming_position == incoming_count) {
        return 0U;
    }
    const radio_frame *frame = &incoming[incoming_position++];
    CHECK(frame->length <= capacity);
    memcpy(bytes, frame->bytes, frame->length);
    return frame->length;
}

static void reset_radio(void) {
    incoming_count = 0U;
    incoming_position = 0U;
    outgoing_count = 0U;
    lora_link_init();
}

static void queue_packet(uint8_t type, uint8_t network, uint8_t source, uint8_t destination,
                         uint8_t packet_id) {
    CHECK(incoming_count < RADIO_QUEUE_CAPACITY);
    lora_packet_t packet;
    lora_packet_init(&packet);
    packet.packet_type = type;
    packet.network_id = network;
    packet.source_id = source;
    packet.destination_id = destination;
    packet.packet_id = packet_id;
    radio_frame *frame = &incoming[incoming_count++];
    frame->length = lora_packet_encode(&packet, frame->bytes, sizeof frame->bytes);
    CHECK(frame->length == LORA_PACKET_HEADER_SIZE);
}

static void test_packet_boundaries(void) {
    for (unsigned size = 0U; size <= LORA_PACKET_MAX_PAYLOAD_SIZE; size++) {
        lora_packet_t packet;
        lora_packet_init(&packet);
        packet.source_id = LORA_NODE_TX_ID;
        packet.destination_id = LORA_NODE_RX_ID;
        packet.payload_length = (uint8_t)size;
        for (unsigned index = 0U; index < size; index++) {
            packet.payload[index] = (uint8_t)(index + 1U);
        }
        uint8_t encoded[LORA_PACKET_MAX_ENCODED_SIZE + 1U];
        memset(encoded, 0xA5, sizeof encoded);
        uint8_t length = lora_packet_encode(&packet, encoded, sizeof encoded);
        CHECK(length == LORA_PACKET_HEADER_SIZE + size);
        CHECK(encoded[length] == 0xA5);
        lora_packet_t decoded;
        uint8_t result = lora_packet_decode(&decoded, encoded, length);
        CHECK(result == 1U);
        CHECK(memcmp(&packet, &decoded, sizeof packet) == 0);

        for (unsigned truncated = 0U; truncated < length; truncated++) {
            memset(&decoded, 0xA5, sizeof decoded);
            const lora_packet_t previous = decoded;
            result = lora_packet_decode(&decoded, encoded, (uint8_t)truncated);
            CHECK(result == 0U);
            CHECK(memcmp(&decoded, &previous, sizeof decoded) == 0);
        }
        result = lora_packet_decode(&decoded, encoded, (uint8_t)(length + 1U));
        CHECK(result == 0U);

        memset(encoded, 0xA5, sizeof encoded);
        result = lora_packet_encode(&packet, encoded, (uint8_t)(length - 1U));
        CHECK(result == 0U);
        for (unsigned index = 0U; index < sizeof encoded; index++) {
            CHECK(encoded[index] == 0xA5);
        }
    }

    lora_packet_t packet;
    lora_packet_init(&packet);
    uint8_t bytes[LORA_PACKET_MAX_ENCODED_SIZE] = {0};
    packet.payload_length = LORA_PACKET_MAX_PAYLOAD_SIZE + 1U;
    uint8_t result = lora_packet_encode(&packet, bytes, sizeof bytes);
    CHECK(result == 0U);
    packet.payload_length = 0U;
    packet.packet_type = 0xFFU;
    result = lora_packet_encode(&packet, bytes, sizeof bytes);
    CHECK(result == 0U);
    result = lora_packet_encode(NULL, bytes, sizeof bytes);
    CHECK(result == 0U);
    result = lora_packet_decode(&packet, NULL, sizeof bytes);
    CHECK(result == 0U);
    bytes[4] = LORA_PACKET_TYPE_DATA;
    bytes[5] = LORA_PACKET_MAX_PAYLOAD_SIZE + 1U;
    result = lora_packet_decode(&packet, bytes, sizeof bytes);
    CHECK(result == 0U);
}

static void test_responses_match_the_peer(void) {
    const uint8_t payload[] = {0x12U, 0x34U};
    const uint8_t response_types[] = {LORA_PACKET_TYPE_ACK, LORA_PACKET_TYPE_PONG};
    for (unsigned index = 0U; index < sizeof response_types; index++) {
        const uint8_t type = response_types[index];
        reset_radio();
        queue_packet(type, 0U, LORA_NODE_RX_ID, LORA_NODE_TX_ID, 0U);
        queue_packet(type, LORA_NETWORK_ID, 3U, LORA_NODE_TX_ID, 0U);
        queue_packet(type, LORA_NETWORK_ID, LORA_NODE_RX_ID, 3U, 0U);
        queue_packet(type, LORA_NETWORK_ID, LORA_NODE_RX_ID, LORA_NODE_TX_ID, 1U);
        queue_packet(type, LORA_NETWORK_ID, LORA_NODE_RX_ID, LORA_NODE_TX_ID, 0U);
        uint8_t result;
        if (type == LORA_PACKET_TYPE_ACK) {
            result = lora_link_send_reliable(LORA_NODE_RX_ID, payload, sizeof payload);
        } else {
            result = lora_link_send_ping(LORA_NODE_RX_ID);
        }
        CHECK(result == LORA_LINK_STATUS_OK);
        CHECK(incoming_position == incoming_count);
        CHECK(outgoing_count == 1U);
    }
}

static void test_failure_and_receiver_paths(void) {
    uint8_t payload[LORA_PACKET_MAX_PAYLOAD_SIZE + 1U] = {0};
    reset_radio();
    uint8_t result = lora_link_send_reliable(LORA_NODE_RX_ID, payload, sizeof payload);
    CHECK(result == LORA_LINK_STATUS_FAILED);
    CHECK(outgoing_count == 0U);
    result = lora_link_send_reliable(LORA_NODE_RX_ID, payload, 1U);
    CHECK(result == LORA_LINK_STATUS_FAILED);
    CHECK(outgoing_count == 3U);

    reset_radio();
    queue_packet(LORA_PACKET_TYPE_DATA, LORA_NETWORK_ID, LORA_NODE_TX_ID, LORA_NODE_RX_ID, 9U);
    result = lora_link_receive_and_ack();
    CHECK(result == 1U);
    CHECK(outgoing_count == 1U);
    CHECK(outgoing[0].bytes[4] == LORA_PACKET_TYPE_ACK);
    CHECK(outgoing[0].bytes[2] == LORA_NODE_TX_ID);
    CHECK(outgoing[0].bytes[3] == 9U);

    reset_radio();
    queue_packet(LORA_PACKET_TYPE_PING, LORA_NETWORK_ID, LORA_NODE_TX_ID, LORA_NODE_RX_ID, 7U);
    result = lora_link_receive_ping_and_respond();
    CHECK(result == 1U);
    CHECK(outgoing[0].bytes[4] == LORA_PACKET_TYPE_PONG);
    CHECK(outgoing[0].bytes[3] == 7U);

    reset_radio();
    queue_packet(LORA_PACKET_TYPE_DATA, 0U, LORA_NODE_TX_ID, LORA_NODE_RX_ID, 0U);
    result = lora_link_receive_and_ack();
    CHECK(result == 0U);
    CHECK(outgoing_count == 0U);
}

int main(void) {
    test_packet_boundaries();
    test_responses_match_the_peer();
    test_failure_and_receiver_paths();
    printf("PASS: %u protocol checks; radio I/O is simulated\n", checks);
    return EXIT_SUCCESS;
}
