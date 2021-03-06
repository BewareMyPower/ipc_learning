// psem_wait.cc
#include <unistd.h>
#include "sem_util.h"

int main(int argc, char* argv[]) {
  if (argc < 2 || strcmp(argv[1], "--help") == 0)
    errorExit("Usage: %s sem-name", argv[0]);

  auto sem = SemOpen(argv[1], 0);

  if (sem_wait(sem) == -1) errorExit(errno, "sem_wait");
  printf("%ld psem_wait() succeeded\n", (long)getpid());
  return 0;
}
