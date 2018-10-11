// pshm_read.cc
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "error_handler.h"

int main(int argc, char* argv[]) {
  if (argc < 2) errorExit("Usage: %s shm-name", argv[0]);

  int fd = shm_open(argv[1], O_RDONLY, 0);
  if (fd == -1) errorExit(errno, "shm_open");

  struct stat statbuf;
  if (fstat(fd, &statbuf) == -1) errorExit(errno, "fstat");

  auto len = static_cast<long>(statbuf.st_size);
  printf("Shared memory(\"%s\")'s size: %ld\n", argv[1], len);

  void* addr = mmap(nullptr, len, PROT_READ, MAP_SHARED, fd, 0);
  if (addr == MAP_FAILED) errorExit(errno, "mmap");

  if (close(fd) == -1) errorExit(errno, "close");

  write(STDOUT_FILENO, addr, len);
  printf("\n");
  return 0;
}
