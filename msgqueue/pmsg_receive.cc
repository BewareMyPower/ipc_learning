// pmsg_receive.cc
#include <mqueue.h>
#include <unistd.h>
#include "error_handler.hpp"

inline void usageError(const char* name) {
    errorExit("Usage: %s [-n] mq_name\n"
              "    -n        Use O_NONBLOCK flag\n",
              name);
}

int main(int argc, char* argv[]) {
    int flags = O_RDONLY;

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

    if (optind >= argc) {
        errorMsg("mq_name and message are both required");
        usageError(argv[0]);
    }

    // 打开管道
    auto mqd = mq_open(argv[optind], flags);
    if (mqd == -1) errorExit(errno, "mq_open");

    // 获取管道属性
    struct mq_attr attr;
    if (mq_getattr(mqd, &attr) == -1)
        errorExit(errno, "mq_getattr");

    // 构造容纳消息大小的缓冲区来接收数据
    char* buffer = new char[attr.mq_msgsize + 1];
    unsigned int prio;
    ssize_t num_read = mq_receive(mqd, buffer, attr.mq_msgsize, &prio);
    if (num_read == -1) errorExit(errno, "mq_receive");

    printf("receive %ld bytes; priority = %u\n", static_cast<long>(num_read), prio);
    buffer[num_read] = '\0';
    printf("%s\n", buffer);

    return 0;
}
