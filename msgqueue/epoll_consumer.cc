// epoll_consumer.cc: 使用epoll来等待消息队列上的读消息
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include "mqueue_wrapper.h"
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
  std::unordered_map<int, std::unique_ptr<MQueue>> mq_map;
  for (int i = 1; i < argc; i++) {
    std::unique_ptr<MQueue> mq_ptr{new MQueue(argv[i], O_RDONLY | O_NONBLOCK)};
    mq_map.emplace(mq_ptr->fd, std::move(mq_ptr));
  }

  // 创建I/O多路复用对象监听各个消息队列
  Poller poller;
  for (auto& kv : mq_map) poller.add(kv.first, EPOLLIN);
  poller.registerInterruptSignal(SIGINT);

  // 监听读事件并处理
  while (true) {
    auto events = poller.poll(-1);
    if (events.size() == 0) break;

    // 消费消息队列
    for (auto& ev : events) {
      auto& mq_ptr = mq_map.at(ev.data.fd);
      if (!mq_ptr) errorExit("fd: %d associate with nullptr", ev.data.fd);
      while (mq_ptr->receive() >= 0)
        printf("%s receives: %s\n", mq_ptr->name.data(), mq_ptr->buffer.data());
    }
  }

  printf("Stop receiving\n");
  return 0;
}
