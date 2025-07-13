#ifndef __DEBUG_H__
#define __DEBUG_H__


#include <stdio.h>
#include <stdlib.h>

#define Assert(expr, fmt, ...) \
    do { \
        if (!(expr)) { \
            fprintf(stderr, "\033[1;31m[ASSERT FAILED]\033[0m\n"); \
            fprintf(stderr, "  Expression: %s\n", #expr); \
            fprintf(stderr, "  Location  : %s:%d (%s)\n", __FILE__, __LINE__, __func__); \
            fprintf(stderr, "  Message   : " fmt "\n", ##__VA_ARGS__); \
            abort(); \
        } \
    } while (0)

#define Log(format, ...) \
    _Log(ANSI_FMT("[%s:%d %s] " format, ANSI_FG_BLUE) "\n", \
        __FILE__, __LINE__, __func__, ## __VA_ARGS__)


#endif
