#ifndef RINGBUFFER_H
#define RINGBUFFER_H

typedef struct {
    char *buffer;          // 原始内存
    int   entry_size;      // 每条 entry 字节数
    int   capacity;        // 最大可存条数
    int   start;           // 指向“最旧条目”
    int   size;            // 当前有效条目个数 (0~capacity)
} RingBufferEntry;

/* API */
RingBufferEntry *rb_entry_create(int entry_size, int num_entries);
void             rb_entry_destroy(RingBufferEntry *rb);

/* 写 1 条；永远成功，必要时覆盖最旧条目 */
void rb_entry_write(RingBufferEntry *rb, const void *entry);

/* 读 n 条到 dst（dst 至少 n*entry_size 字节），返回实际读出的条数 */
int  rb_entry_read(RingBufferEntry *rb, void *dst, int n);

/* 工具 */
static inline int rb_entry_used(RingBufferEntry *rb)  { return rb->size;      }
static inline int rb_entry_free(RingBufferEntry *rb)  { return rb->capacity - rb->size; }


#endif // RINGBUFFER_H
