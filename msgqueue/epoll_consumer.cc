// epoll_consumer.cc: 使用epoll来等待消息队列上的读消息
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include "mqueue_util.hpp"
#include "poller.h"

struct MQueue {
    MQueue(const char* mq_name, int oflag) : name(mq_name) {
        fd = MqOpen(mq_name, oflag);
        auto attr = MqGetAttr(fd);
        buffer.resize(attr.mq_msgsize + 1, '\0');  // 多1个给'\0'
    }

    ssize_t receive(unsigned int* prio = nullptr) noexcept {
        ssize_t n = MqReceive(fd, buffer.data(), buffer.size(), prio);
        if (n > 0)
            buffer[n] = '\0';
        else
            buffer[0] = '\0';
        return n;
    }

    int fd;
    std::string name;
    std::vector<char> buffer;
};

int main(int argc, char* argv[]) {
    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        errorExit("Usage: %s mq-name...", argv[0]);

    // 打开消息队列并初始化相关数据
    std::unordered_map<int, std::unique_ptr<MQueue>> mqMap;
    for (int i = 1; i < argc; i++) {
        std::unique_ptr<MQueue> mqPtr{new MQueue(argv[i], O_RDONLY | O_NONBLOCK)};
        mqMap.emplace(mqPtr->fd, std::move(mqPtr));
    }

    // 创建I/O多路复用对象监听各个消息队列
    Poller poller;
    for (auto& kv : mqMap)
        poller.add(kv.first, EPOLLIN);
    poller.registerInterruptSignal(SIGINT);

    // 监听读事件并处理
    while (true) {
        auto events = poller.poll(-1);
        if (events.size() == 0)
            break;

        // 消费消息队列
        for (auto& ev : events) {
            auto& mqPtr = mqMap.at(ev.data.fd);
            if (!mqPtr)
                errorExit("fd: %d associate with nullptr", ev.data.fd);
            while (mqPtr->receive() >= 0)
                printf("%s receives: %s\n", mqPtr->name.data(), mqPtr->buffer.data());
        }
    }

    printf("Stop receiving\n");
    return 0;
}
