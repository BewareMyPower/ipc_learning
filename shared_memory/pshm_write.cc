// pshm_write.cc
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "error_handler.hpp"

int main(int argc, char* argv[]) {
    if (argc < 3)
        errorExit("Usage: %s shm-name write", argv[0]);

    int fd = shm_open(argv[1], O_RDWR, 0);
    if (fd == -1)
        errorExit(errno, "shm_open");

    size_t len = strlen(argv[2]);
    if (ftruncate(fd, len) == -1)  // 设置共享内存的大小以容纳字符串
        errorExit("ftruncate");
    printf("Resized to %ld bytes\n", static_cast<long>(len));

    void* addr = mmap(nullptr, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED)
        errorExit(errno, "mmap");

    if (close(fd) == -1)
        errorExit(errno, "close");

    printf("copying %ld bytes\n", static_cast<long>(len));
    memcpy(addr, argv[2], len);  // 拷贝字符串到共享内存中
    return 0;
}
