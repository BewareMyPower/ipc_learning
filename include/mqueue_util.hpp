// mqueue_util.hpp: 对<mqueue.h>的错误包裹函数
// Linux上mqd_t即int，因此所有包裹函数中传入mqd_t的地方用int代替
#pragma once
#include <mqueue.h>
#include "error_handler.hpp"

inline int
MqCreate(const char* name, int oflag, mode_t perm,
         long maxmsg = 100, long msgsize = 1024) noexcept {
    struct mq_attr attr;
    attr.mq_maxmsg = maxmsg;
    attr.mq_msgsize = msgsize;

    int mqd = mq_open(name, oflag | O_CREAT, perm, &attr);
    if (mqd == -1)
        errorExit(errno, "mq_open CREAT %s", name);
    return mqd;
}

inline int
MqOpen(const char* name, int oflag) noexcept {
    int mqd = mq_open(name, oflag);
    if (mqd == -1)
        errorExit(errno, "mq_open %s", name);
    return mqd;
}

inline void
MqUnlink(const char* name) noexcept {
    if (mq_unlink(name) == -1)
        errorExit(errno, "mq_unlink %s", name);
}

inline struct mq_attr
MqGetAttr(int mqd) noexcept {
    struct mq_attr attr;
    if (mq_getattr(mqd, &attr) == -1)
        errorExit(errno, "mq_getattr");
    return attr;
}

inline void
MqSetAttr(int mqd, const struct mq_attr& attr) noexcept {
    if (mq_setattr(mqd, &attr, nullptr) == -1)
        errorExit(errno, "mq_setattr");
}

inline ssize_t
MqReceive(int mqd, char* buf, size_t bufsize,
          unsigned int* prio = nullptr) noexcept {
    ssize_t n = mq_receive(mqd, buf, bufsize, prio);
    if (n == -1 && errno != EAGAIN)
        errorExit(errno, "[mqd: %d] mq_receive", mqd);
    return n;
}

inline ssize_t
MqSend(int mqd, const char* buf, size_t bufsize,
       unsigned int prio = 0) noexcept {
    ssize_t n = mq_send(mqd, buf, bufsize, prio);
    if (n == -1)
        errorExit(errno, "mq_send");
    return n;
}
