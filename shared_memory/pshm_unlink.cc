// pshm_unlink.cc
#include <fcntl.h>
#include <sys/mman.h>
#include "error_handler.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2)
        errorExit("Usage: %s shm-name", argv[0]);
    if (shm_unlink(argv[1]) == -1)
        errorExit(errno, "shm_unlink");
    return 0;
}
