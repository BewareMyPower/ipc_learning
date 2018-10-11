// pmsg_send.cc
#include <mqueue.h>
#include <unistd.h>
#include "error_handler.h"

inline void usageError(const char* name) {
  errorExit(
      "Usage: %s [-n] mq_name msg [prio]\n"
      "    -n        Use O_NONBLOCK flag\n",
      name);
}

int main(int argc, char* argv[]) {
  int flags = O_WRONLY;

  // 解析命令行参数
  int opt;
  while ((opt = getopt(argc, argv, "n")) != -1) {
    switch (opt) {
      case 'n':
        flags |= O_NONBLOCK;
        break;
      default:
        usageError(argv[0]);
    }
  }

  if (optind + 1 >= argc) {
    errorMsg("mq_name and message are both required\n");
    usageError(argv[0]);
  }

  // 打开管道
  auto mqd = mq_open(argv[optind], flags);
  if (mqd == -1) errorExit(errno, "mq_open");

  // 发送消息
  unsigned int prio = (argc > optind + 2) ? atoi(argv[optind + 2]) : 0;
  if (mq_send(mqd, argv[optind + 1], strlen(argv[optind + 1]), prio) == -1)
    errorExit(errno, "mq_send");

  return 0;
}
