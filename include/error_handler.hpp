// error_handler.hpp
#pragma once

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

template <typename ...Args>
inline void errorMsg(int error, const char* format, Args... args) {
    char buf[1024];
    char error_buf[128];
    snprintf(buf, sizeof(buf), format, args...);

    size_t n = strlen(buf);
    if (error) {
        snprintf(buf + n, sizeof(buf) - n, ": %s",
                 strerror_r(error, error_buf, sizeof(error_buf)));
    }

    strcat(buf, "\n");
    fputs(buf, stderr);
}

inline void errorMsg(int error, const char* message) {
    errorMsg(error, "%s", message);
}

template <typename ...Args>
inline void errorMsg(const char* format, Args... args) {
    errorMsg(0, format, args...);
}

inline void errorMsg(const char* message) {
    errorMsg(0, message);
}

template <typename ...Args>
inline void errorExit(Args... args) {
    errorMsg(args...);
    exit(1);
}
