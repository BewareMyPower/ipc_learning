// mq_util.h
#pragma once
#include <mqueue.h>
#include "error_handler.h"

// NOTE: int is equivalent to mqd_t on Linux
static_assert(sizeof(int) == sizeof(mqd_t), "can't treat mqd_t as int");

constexpr int MQ_USER_RW_PERMS = S_IRUSR | S_IWUSR;

inline int MqOpen(const char* name, int oflag) noexcept {
  int mqd = mq_open(name, oflag);
  if (mqd == -1) errorExit(errno, "mq_open \"%s\"", name);
  return mqd;
}

inline int MqCreate(const char* name, int oflag, int perms,
                    const struct mq_attr& attr) noexcept {
  int mqd = mq_open(name, oflag | O_CREAT, perms, &attr);
  if (mqd == -1) errorExit(errno, "mq_open \"%s\" CREAT", name);
  return mqd;
}

inline void MqUnlink(const char* name) noexcept {
  if (mq_unlink(name) == -1) errorExit(errno, "mq_unlink");
}

inline void MqSetAttr(int mqd, const struct mq_attr& attr) noexcept {
  if (mq_setattr(mqd, &attr, nullptr) == -1) errorExit(errno, "mq_setattr");
}

inline struct mq_attr MqGetAttr(int mqd) noexcept {
  struct mq_attr attr;
  if (mq_getattr(mqd, &attr) == -1) errorExit("mq_getattr");
  return attr;
}

inline void MqSetNonblocking(int mqd) noexcept {
  struct mq_attr attr;
  attr.mq_flags |= O_NONBLOCK;
  MqSetAttr(mqd, attr);
}
