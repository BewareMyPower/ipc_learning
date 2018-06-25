// pmsg_unlink.cc
#include <mqueue.h>

#include "include/error_handler.hpp"
using namespace error_handler;

int main(int argc, char* argv[]) {
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        errorExit("%s mq-name", argv[0]);
    if (mq_unlink(argv[1]) == -1)
        errorExit(errno, "mq_unlink");
    return 0;
}
