// psem_unlink.cc
#include "sem_util.h"

int main(int argc, char* argv[]) {
  if (argc != 2 || strcmp(argv[1], "--help") == 0)
    errorExit("Usage: %s sem-name", argv[0]);

  SemUnlink(argv[1]);
  return 0;
}
