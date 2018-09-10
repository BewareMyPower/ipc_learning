// pmsg_getattr.cc
#include <mqueue.h>
#include "error_handler.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        errorExit("usage: %s mq-name", argv[0]);

    auto mqd = mq_open(argv[1], O_RDONLY);  // 消息队列描述符
    if (mqd == -1)
        errorExit(errno, "mq_open");

    struct mq_attr attr;
    if (mq_getattr(mqd, &attr) == -1)
       errorExit("mq_getattr");

    printf("Maximum # of messages on queue:   %ld\n", attr.mq_maxmsg);
    printf("Maximum message size:             %ld\n", attr.mq_msgsize);
    printf("# of messages currently on queue: %ld\n", attr.mq_curmsgs);
    return 0;
}
