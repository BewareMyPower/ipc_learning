// psem_getvalue.cc
#include "sem_util.h"

int main(int argc, char* argv[]) {
    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        errorExit("Usage: %s sem-name", argv[0]);

    auto sem = SemOpen(argv[1], 0);

    int val;
    if (sem_getvalue(sem, &val) == -1)
        errorExit(errno, "sem_getvalue()");

    printf("%d\n", val);
    return 0;
}
