/*========================================================================
 *  iritrace.c  ——  实现与 iritrace.h 配套的环形 Trace 记录模块
 *  依赖：RingBuffer_entry.h / RingBuffer_entry.c
 *========================================================================*/
#include <cpu/iritrace.h>
#include "RingBuffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* --------------------------------------------------------------------- */
/*  内部数据                                                              */
/* --------------------------------------------------------------------- */
static RingBufferEntry *trace_buf = NULL;
static const int ENTRY_SIZE = sizeof(IriTraceEntry);

/* --------------------------------------------------------------------- */
/*  初始化：创建可容纳 num_entries 条记录的环形缓冲区                     */
/* --------------------------------------------------------------------- */
void init_iritrace(int num_entries)
{
    int capacity = num_entries + 1;
    trace_buf = rb_entry_create(ENTRY_SIZE, capacity);
    assert(trace_buf && "init_iritrace: cannot create ring buffer");

    printf("[iritrace] buffer ready, entry_size=%d, capacity=%d\n",
           ENTRY_SIZE, capacity);
}

/* --------------------------------------------------------------------- */
/*  记录一条 Trace（自动覆盖最旧记录，永不失败）                          */
/* --------------------------------------------------------------------- */
void iritrace_record(uint32_t pc, const char *inst_str,
                     uint32_t cycle, uint32_t *regs)
{
    assert(trace_buf && "iritrace not initialized");

    IriTraceEntry e;
    e.pc = (uint32_t)pc;                                   /* 截断到 32 位 */
    strncpy(e.char_inst, inst_str, MAX_INST_STR_LEN - 1);
    e.char_inst[MAX_INST_STR_LEN - 1] = '\0';
    e.cycle = cycle;
    memcpy(e.regs, regs, sizeof(e.regs));

    rb_entry_write(trace_buf, &e);                         /* 覆盖式写入 */
}

/* --------------------------------------------------------------------- */
/*  打印当前缓冲区内全部 Trace（不清空）                                  */
/* --------------------------------------------------------------------- */
static inline const IriTraceEntry *
nth_entry(const RingBufferEntry *rb, int n)   /* n: 0..size-1 */
{
    int idx = (rb->start + n) % rb->capacity;
    return (const IriTraceEntry *)(rb->buffer + idx * rb->entry_size);
}

const char *iritrace_regs[] = {
    "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
};

void iritrace_print(void)
{
    assert(trace_buf && "iritrace not initialized");

    int total = rb_entry_used(trace_buf);
    printf("[iritrace] total %d entry(s)\n", total);

    for (int i = 0; i < total; i++) {
        const IriTraceEntry *p = nth_entry(trace_buf, i);

        printf("CYC:%-10lu  PC:0x%.8x %s\n",
               p->cycle, p->pc, p->char_inst);

        printf("  REGS:");
        for (int r = 0; r < 16; r++) {
            printf(" %-2s:%08x", iritrace_regs[r], p->regs[r]);
            if ((r + 1) % 4 == 0) putchar('\n'), printf("       ");
        }
        putchar('\n');
        puts("------------------------------------------------------------");
    }
}

/* --------------------------------------------------------------------- */
/*  dump 全部 Trace 到二进制文件（不清空）                                */
/* --------------------------------------------------------------------- */
void iritrace_dump(const char *filename)
{
    assert(trace_buf && "iritrace not initialized");

    FILE *fp = fopen(filename, "wb");
    if (!fp) { perror("iritrace_dump fopen"); return; }

    int total = rb_entry_used(trace_buf);
    for (int i = 0; i < total; i++) {
        const IriTraceEntry *p = nth_entry(trace_buf, i);
        fwrite(p, ENTRY_SIZE, 1, fp);
    }
    fclose(fp);

    printf("[iritrace] dumped %d entry(s) to %s\n", total, filename);
}

/* --------------------------------------------------------------------- */
/*  销毁                                                                  */
/* --------------------------------------------------------------------- */
void iritrace_destroy(void)
{
    rb_entry_destroy(trace_buf);
    trace_buf = NULL;
}
