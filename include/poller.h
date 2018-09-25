// poller.h
#pragma once
#include <signal.h>
#include <stdint.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <utility>
#include <vector>

#include "error_handler.hpp"

class Poller final {
public:
    using Event = struct epoll_event;
    using EventList = std::vector<Event>;

    // 创建epoll描述符，若创建失败则报错并终止进程
    Poller() noexcept {
        epfd_ = epoll_create(1);
        if (epfd_ == -1)
            errorExit(errno, "epoll_create");
    }

    // 关闭epoll描述符
    ~Poller() {
        close(epfd_);
    }

    // 添加监听的描述符fd到监听列表中
    void add(int fd, uint32_t events) noexcept {
        Event ev;
        ev.events = events;
        ev.data.fd = fd;

        if (epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev) == -1)
            errorExit(errno, "epoll_ctl ADD %d", fd);
        num_events_++;
    }

    /**
     * 功能: 轮询监听列表得到被触发的事件列表
     * 参数:
     *   timeout_ms: 阻塞的毫秒数，默认设为-1则无限阻塞，设为0即不阻塞立刻返回
     * 返回值:
     *   若轮询成功，则返回得到的事件列表; 否则返回空列表;
     *   若出现致命性错误则报错并退出程序。
     * 说明:
     *   如果timeout_ms不为默认值，即使轮询成功也可能返回空列表
     *   此时需要检查errno来判断出错原因是轮询错误还是暂无事件被触发
     */
    EventList poll(int timeout_ms = -1) const noexcept {
        int old_errno = errno;
        EventList events(num_events_);

        do_poll_ = true;
        while (do_poll_) {
            int num_wait = epoll_wait(epfd_, events.data(), events.size(), timeout_ms);
            if (num_wait < 0) {
                if (errno == EINTR) {  // 重启被信号中断的epoll_wait
                    errno = old_errno;
                    continue;
                } else {  // 致命性错误
                    errorExit(errno, "epoll_wait");
                }
            } else if (num_wait == 0) {
                events.clear();
            } else {
                events.resize(static_cast<size_t>(num_wait));
            }
            break;
        }
        return do_poll_ ? events : EventList{};
    }

    // 注册中断信号，用于中断poll()方法
    void registerInterruptSignal(int signo) {
        if (::signal(signo, [](int) { do_poll_ = false; }) == SIG_ERR)
            errorExit(errno, "signal %d", signo);
    }

private:
    int epfd_;
    int num_events_ = 0;  // 监听列表数量
    static volatile bool do_poll_;  // 是否等待
};

volatile bool Poller::do_poll_ = false;
