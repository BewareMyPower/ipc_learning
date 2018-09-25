// epoll_consumer.cc: 使用epoll来等待消息队列上的读消息
#include <mqueue.h>
#include <signal.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <string>
#include <unordered_map>
#include "error_handler.hpp"
#include "poller.h"

int main(int argc, char* argv[]) {
    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        errorExit("Usage: %s mq-name...", argv[0]);

    std::vector<int> mqds;  // 消息队列描述符列表
    // 描述符到消息队列名称的映射
    std::unordered_map<int, std::string> mqd_to_name;
    // 描述符到消息大小的映射
    std::unordered_map<int, size_t> mqd_to_bufsize;

    std::vector<char> buffer(1024);  // 消息缓冲区

    struct mq_attr attr;
    for (int i = 1; i < argc; i++) {
        // 创建消息队列
        const char* mq_name = argv[i];
        int mqd = mq_open(mq_name, O_RDONLY | O_NONBLOCK);
        if (mqd == -1)
            errorExit(errno, "mq_open \"%s\"", mq_name);

        // 取得消息最大大小
        if (mq_getattr(mqd, &attr) == -1)
            errorExit(errno, "mq_getattr %d %s", mqd, mq_name);

        // 更新消息队列相应结构
        mqd_to_name.emplace(mqd, mq_name);
        mqd_to_bufsize.emplace(mqd, attr.mq_msgsize);
        mqds.emplace_back(mqd);

        // 确立合适的缓冲区大小
        if (attr.mq_msgsize >= static_cast<long>(buffer.size()))
            buffer.resize(attr.mq_msgsize + 1);
    }

    // 创建epoll对象并监听消息队列
    Poller poller;
    for (int mqd : mqds)
        poller.add(mqd, EPOLLIN);
    poller.registerInterruptSignal(SIGINT);

    // 监听读取事件
    ssize_t num_read;
    while (true) {
        auto events = poller.poll(-1);
        if (events.size() == 0)
            break;

        // 消费消息队列
        for (auto& ev : events) {
            int mqd = ev.data.fd;
            while ((num_read = mq_receive(mqd, &buffer[0], mqd_to_bufsize.at(mqd), NULL)) >= 0) {
                buffer[num_read] = '\0';
                printf("%s receives: %s\n", mqd_to_name[mqd].c_str(), buffer.data());
            }
            if (errno != EAGAIN)
                errorExit(errno, "mq_receive");
        }
    }

    printf("Stop receiving\n");
    return 0;
}
