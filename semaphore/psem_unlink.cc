// psem_unlink.cc
#include <semaphore.h>

#include "include/error_handler.hpp"

using namespace error_handler;

int main(int argc, char* argv[]) {
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        errorExit("Usage: %s sem-name", argv[0]);

    if (sem_unlink(argv[1]) == -1)
        errorExit(errno, "sem_unlink \"%s\"", argv[1]);

    return 0;
}

