// sv_ipc.h
#pragma once
#include <fcntl.h>
#include <sys/ipc.h>
#include "error_handler.hpp"

// "rw--w----"
constexpr int IPC_PERM = S_IRUSR | S_IWUSR | S_IWGRP;

inline key_t Ftok(const char* pathname, int proj_id) {
    auto key = ftok(pathname, proj_id);
    if (key == -1)
        errorExit(errno, "ftok(\"%s\", %d)", pathname, proj_id);
    return key;
}
