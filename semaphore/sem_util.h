// sem_util.h
#pragma once

#include <semaphore.h>
#include <fcntl.h>

#include "include/error_handler.hpp"
using namespace error_handler;

constexpr mode_t DEFAULT_PERM = S_IRUSR | S_IWUSR;  // 默认创建权限"rw-------"

inline sem_t* SemOpen(const char* name, int oflag,
                     mode_t mode = DEFAULT_PERM,
                     unsigned int value = 0) {
    auto sem = sem_open(name, oflag, mode, value);
    if (sem == SEM_FAILED)
        errorExit(errno, "sem_open \"%s\"", name);
    return sem;
}

inline void SemUnlink(const char* name) {
    if (sem_unlink(name) == -1)
        errorExit(errno, "sem_unlink");
}
