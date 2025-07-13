#ifndef __dbg_h__
#define __dbg_h__

#include <stdio.h>
#include <errno.h>
#include <string.h>

// 获取 errno 信息
#define clean_errno() (errno == 0 ? "None" : strerror(errno))

// 基础日志宏
#define log_err(M, ...) fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", \
    __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_warn(M, ...) fprintf(stderr, "[WARN ] (%s:%d: errno: %s) " M "\n", \
    __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_info(M, ...) fprintf(stderr, "[INFO ] (%s:%d) " M "\n", \
    __FILE__, __LINE__, ##__VA_ARGS__)

#define debug(M, ...) fprintf(stderr, "[DEBUG] (%s:%d:%s) " M "\n", \
    __FILE__, __LINE__, __func__, ##__VA_ARGS__)

// 错误检查宏
#define check(A, M, ...) \
    if (!(A)) { \
        log_err(M, ##__VA_ARGS__); errno = 0; goto error; \
    }

#define check_debug(A, M, ...) \
    if (!(A)) { \
        debug(M, ##__VA_ARGS__); errno = 0; goto error; \
    }

// 内存分配检查
#define check_mem(A) check((A), "Out of memory.")

// 标记不应该执行到的位置
#define sentinel(M, ...)  { log_err(M, ##__VA_ARGS__); errno = 0; goto error; }

#endif
