// epoll_consumer.cc: 使用epoll来等待消息队列上的读消息
#include <mqueue.h>
#include <signal.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <string>
#include "error_handler.hpp"

class MyData {
public:
    MyData(int fd, const std::string& name) : fd_(fd), name_(name) {}

    int getFd() const { return fd_; }
    std::string getName() const { return name_ + "[" + std::to_string(fd_) + "]"; }

private:
    int fd_;
    std::string name_;
};

static volatile bool do_receive = true;

int main(int argc, char* argv[]) {
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        errorExit("Usage: %s mq-name", argv[0]);

    // 创建消息队列
    auto mqd = mq_open(argv[1], O_RDONLY | O_NONBLOCK);
    if (mqd == -1) errorExit(errno, "mq_open \"%s\"", argv[0]);

    // 创建合适的缓冲区
    struct mq_attr attr;
    if (mq_getattr(mqd, &attr) == -1)
        errorExit(errno, "mq_getattr");
    char* buffer = new char[attr.mq_msgsize + 1];

    // 消息队列关联数据
    MyData mqdData{mqd, "Message Queue " + std::to_string(mqd)};

    // 创建epoll对象
    auto epfd = epoll_create1(0);

    // 监听消息队列描述符和关联数据结构
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.ptr = &mqdData;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, mqd, &ev) == -1)
        errorExit(errno, "epoll_ctl");

    // 用信号Ctrl+C来安全终止循环
    signal(SIGINT, [](int) { do_receive = false; });

    // 监听读取事件
    ssize_t num_read;
    while (do_receive) {
epoll_wait_again:
        if (epoll_wait(epfd, &ev, 1, -1) == -1) {
            if (errno == EINTR) {
                if (do_receive)
                    goto epoll_wait_again;
                else  // SIGINT信号
                    break;
            }
            else
                errorExit(errno, "epoll_wait");
        }

        // 消费消息队列
        while ((num_read = mq_receive(mqd, buffer, attr.mq_msgsize, NULL)) >= 0) {
            buffer[num_read] = '\0';
            printf("%s receives: %s\n", mqdData.getName().c_str(), buffer);
        }

        if (errno != EAGAIN) errorExit(errno, "mq_receive");
    }

    printf("%s stop receiving\n", mqdData.getName().c_str());
    return 0;
}
