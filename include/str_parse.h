// str_parse.h
#pragma once
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

inline long StrToLong(const char* str, int base = 10) {
    if (!str) {
        fprintf(stderr, "StrToLong(): str is NULL!\n");
        exit(1);
    }

    errno = 0;
    char* endptr;
    long val = strtol(str, &endptr, base);

    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
            || (errno != 0 && val == 0)) {
        fprintf(stderr, "StrToLong(\"%s\", %d): %s\n", str, base, strerror(errno));
        exit(1);
    }

    while (*endptr != '\0' && isblank(*endptr))
        endptr++;
    if (*endptr != '\0') {
        fprintf(stderr, "StrToLong: \"%s\" contains invalid characters\n", str);
        exit(1);
    }

    return val;
}

class IntRange {
public:
    IntRange() noexcept = default;

    IntRange(int low, int high) noexcept : low_(low), high_(high) {}

    static IntRange UpperBound(int high) noexcept { return {INT_MIN, high}; }
    static IntRange LowerBound(int low) noexcept { return {low, INT_MAX}; }

    bool check(int val) const noexcept {
        return val >= low_ && val <= high_;
    }

    const char* ToCStr() {
        static char buf[30];
        if (low_ == INT_MIN && high_ == INT_MAX) {
            return "(INT_MIN, INT_MAX)";
        } else if (low_ != INT_MIN) {
            snprintf(buf, sizeof(buf), "[%d, INT_MAX)", low_);
        } else if (high_ != INT_MAX) {
            snprintf(buf, sizeof(buf), "(INT_MIN, %d]", high_);
        } else {
            snprintf(buf, sizeof(buf), "[%d, %d]", low_, high_);
        }
        return buf;
    }

private:
    int low_ = INT_MIN;
    int high_ = INT_MAX;
};

inline int StrToInt(const char* str, int base, IntRange range = {}) {
    long val = StrToLong(str, base);
    if (val < INT_MIN || val > INT_MAX || !range.check(val)) {
        fprintf(stderr, "StrToInt(\"%s\", %d): out of range %s\n",
                str, base, range.ToCStr());
        exit(1);
    }
    return val;
}
