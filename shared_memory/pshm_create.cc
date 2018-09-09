// pshm_create.cc
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "include/error_handler.hpp"
using namespace error_handler;

inline void usageError(const char* name) {
    fprintf(stderr,
            "Usage: %s [-cx] name size [octal-perms]\n"
            "    -c   Create shared memory (O_CREAT)\n"
            "    -x   Create exclusively (O_EXCL)\n",
            name);
    exit(1);
}

int main(int argc, char* argv[]) {
    int flags = O_RDWR;
    int opt;
    while ((opt = getopt(argc, argv, "cx")) != -1) {
        switch (opt) {
        case 'c':   flags |= O_CREAT;  break;
        case 'x':   flags |= O_EXCL;   break;
        default:    usageError(argv[0]);
        }
    }

    if (optind + 1 >= argc)
        usageError(argv[0]);
    size_t size = strtoul(argv[optind + 1], nullptr, 10);
    mode_t perms = (argc <= optind + 2) ? (S_IRUSR | S_IWUSR) :
        strtol(argv[optind + 2], nullptr, 8);

    int fd = shm_open(argv[optind], flags, perms);
    if (fd == -1)
        errorExit(errno, "shm_open");

    if (ftruncate(fd, size) == -1)
        errorExit(errno, "ftruncate");

    void* addr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED)
        errorExit(errno, "mmap");

    return 0;
}
