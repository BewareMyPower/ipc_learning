// mq_protocol.h
#pragma once
#include <limits.h>
#include <mqueue.h>
#include <stdint.h>

#pragma pack(1)
struct Message {
  uint32_t id;
  uint8_t length;
  char info[1];  // flexible array
};
#pragma pack()

constexpr size_t MESSAGE_HEADER_SIZE = sizeof(Message) - sizeof(Message::info);
constexpr size_t MAX_INFO_LENGTH = UCHAR_MAX;
constexpr size_t MAX_MESSAGE_SIZE = MESSAGE_HEADER_SIZE + MAX_INFO_LENGTH;
