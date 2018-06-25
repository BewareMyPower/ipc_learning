// sample.cc
#include <mqueue.h>

#include "include/error_handler.hpp"  // include errno.h
using namespace error_handler;

int main() {
    const char* mq_name = "/mq";
    mqd_t mqd;
    struct mq_attr attr;
    ssize_t num_read, num_write;

    // 1. 创建消息队列(若已存在则重新创建)
mq_create_again:
    mqd = mq_open(mq_name, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, nullptr);
    if (mqd == -1) {
        if (errno == EEXIST) {
            mq_unlink(mq_name);
            goto mq_create_again;
        } else {
            errorExit(errno, "mq_open create \"%s\"", mq_name);
        }
    }
    mq_close(mqd);

    // 2. 发送3个消息到消息队列中 
    mqd = mq_open(mq_name, O_WRONLY | O_NONBLOCK);
    if (mqd == -1) errorExit(errno, "mqd_open \"%s\" O_WRONLY | O_NONBLOCK", mq_name);

    constexpr int NUM_MSG = 4;
    const char* messages[NUM_MSG] = {"msg-1", "msg-2", "", "msg-3"};
    int priorities[NUM_MSG] = {4, 0, 1, 6};
    for (int i = 0; i < NUM_MSG; ++i) {
        num_write = mq_send(mqd, messages[i], strlen(messages[i]), priorities[i]);
        if (num_write == -1) errorExit(errno, "mq_send");
    }
    mq_close(mqd);

    // 3. 读取消息队列中的所有消息
    mqd = mq_open(mq_name, O_RDONLY | O_NONBLOCK);
    if (mqd == -1) errorExit(errno, "mqd_open \"%s\" O_RDONLY | O_NONBLOCK", mq_name);

    if (mq_getattr(mqd, &attr) == -1)
       errorExit(errno, "mq_getattr");

    char* buffer = new char[attr.mq_msgsize + 1];
    unsigned int priority;
    while ((num_read = mq_receive(mqd, buffer, attr.mq_msgsize, &priority)) >= 0) {
        buffer[num_read] = '\0';
        printf("[%2u] %s\n", priority, buffer);
    }
    if (errno != EAGAIN) errorExit(errno, "mq_receive");

    delete[] buffer;
    return 0;
}
