#pragma once

#include <stdint.h>

#define LORA_LINK_STATUS_OK 1U
#define LORA_LINK_STATUS_FAILED 0U

void lora_link_init(void);

uint8_t lora_link_send_reliable(uint8_t destination_id, const uint8_t *payload,
                                uint8_t payload_length);

uint8_t lora_link_receive_and_ack(void);

uint8_t lora_link_send_ping(uint8_t destination_id);

uint8_t lora_link_receive_ping_and_respond(void);
