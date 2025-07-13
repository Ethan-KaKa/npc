#ifndef IRITRACE_H
#define IRITRACE_H

#include <stdint.h>

#define MAX_INST_STR_LEN 64

#ifdef __cplusplus
extern "C" {
#endif

// 1. 定义 iritrace entry 结构
typedef struct {
    uint32_t pc;
    char char_inst[MAX_INST_STR_LEN];
    uint64_t cycle;
    uint32_t regs[16];
} IriTraceEntry;

// 2. 初始化 iritrace(见top)


// 3. 记录一条 trace
void iritrace_record(uint32_t pc, const char *inst_str, uint32_t cycle, uint32_t *regs);

// 4. 打印所有 trace 到 stdout
void iritrace_print(void);

// 5. 保存 trace 到文件
void iritrace_dump(const char *filename);

// 6. 释放 iritrace(见top)

#ifdef __cplusplus
}
#endif

#endif // IRITRACE_H
