// pmsg_unlink.cc
#include <mqueue.h>
#include "error_handler.h"

int main(int argc, char* argv[]) {
  if (argc != 2 || strcmp(argv[1], "--help") == 0)
    errorExit("%s mq-name", argv[0]);
  if (mq_unlink(argv[1]) == -1) errorExit(errno, "mq_unlink");
  return 0;
}
