// psem_create.cc
#include <semaphore.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "include/error_handler.hpp"

using namespace error_handler;

static void UsageError(const char* name) {
    errorExit("Usage: %s [-cx] name [octal-perms [value]]\n"
              "    -c   Create semaphore (O_CREAT)\n"
              "    -x   Create exclusively (O_EXCEL)", name);
}

static const char* sem_name = NULL;       // 信号量名
static int flags = 0;                     // 文件打开标志
static mode_t perms = S_IRUSR | S_IWUSR;  // 默认创建权限"rw-------"
static unsigned int sem_value = 0;                 // 信号量初始值

static void ParseArgs(int argc, char* argv[]);

int main(int argc, char* argv[]) {
    ParseArgs(argc, argv);

    sem_t* sem = sem_open(sem_name, flags, perms, sem_value);
    if (sem == SEM_FAILED) errorExit(errno, "sem_open \"%s\"", sem_name);

    return 0;
}

static void ParseArgs(int argc, char* argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "cx")) != -1) {
        switch (opt) {
            case 'c':  flags |= O_CREAT;  break;
            case 'x':  flags |= O_EXCL;   break;
            default:   UsageError(argv[0]);
        }
    }
    if (optind >= argc)
        UsageError(argv[0]);

    sem_name = argv[optind];

    // 判断是否手动设置权限
    if (optind + 1 < argc) {
        perms = strtol(argv[optind + 1], NULL, 8);
        if (perms < 0000 || perms > 0777)
            errorExit("perms must be between 0000 and 0777");
    }

    // 判断是否手动设置初始值
    if (optind + 2 < argc) {
        char* endptr;
        sem_value = strtoul(argv[optind + 2], &endptr, 10);
        if (endptr != NULL)
            errorExit("semaphore value(\"%s\") is invalid", argv[optind + 2]);
    }
}
