// pmsg_create.cc
#include <mqueue.h>
#include <unistd.h>
#include "error_handler.h"

static inline void UsageError(const char* name) {
  errorExit(
      "Usage: %s [-cx] [-m maxmsg] [-s msgsize] mq-name "
      "[octal-perms]\n"
      "     -c         Create queue (O_CREAT)\n"
      "     -m maxmsg  Set maximum # of messages\n"
      "     -s msgsize Set maximum message size\n"
      "     -x         Create exclusively (O_EXCL)\n",
      name);
}

static const char* mq_name = NULL;  // 消息队列名称
static int flags = O_RDWR;          // 创建消息队列的参数
static struct mq_attr attr;         // 消息队列属性
static void ParseArgs(int argc, char* argv[]);

int main(int argc, char* argv[]) {
  // 默认消息队列属性
  attr.mq_maxmsg = 50;
  attr.mq_msgsize = 2048;

  ParseArgs(argc, argv);

  // 创建消息队列
  mode_t perms = (argc <= optind + 1) ? (S_IRUSR | S_IWUSR)
                                      : strtol(argv[optind + 1], NULL, 8);
  mqd_t mqd = mq_open(argv[optind], flags, perms, &attr);
  if (mqd == -1) errorExit(errno, "mq_open");
  return 0;
}

static void ParseArgs(int argc, char* argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "cm:s:x")) != -1) {
    switch (opt) {
      case 'c':
        flags |= O_CREAT;
        break;
      case 'm':
        attr.mq_maxmsg = atoi(optarg);
        break;
      case 's':
        attr.mq_msgsize = atoi(optarg);
        break;
      case 'x':
        flags |= O_EXCL;
        break;
      default:
        UsageError(argv[0]);
    }
  }

  if (optind >= argc) {
    errorMsg("mq-name must be specified!");
    UsageError(argv[0]);
  }
}
