#include "RingBuffer.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

RingBufferEntry *rb_entry_create(int entry_size, int num_entries)
{
    RingBufferEntry *rb = (RingBufferEntry *)malloc(sizeof(*rb));
    assert(rb && "malloc RingBufferEntry failed");

    rb->buffer   = (char *)malloc(entry_size * num_entries);
    assert(rb->buffer && "malloc buffer failed");

    rb->entry_size = entry_size;
    rb->capacity   = num_entries;
    rb->start      = 0;
    rb->size       = 0;
    return rb;
}

void rb_entry_destroy(RingBufferEntry *rb)
{
    if (rb) {
        free(rb->buffer);
        free(rb);
    }
}

static inline int idx(RingBufferEntry *rb, int i)
{ return (rb->start + i) % rb->capacity; }

/* 覆盖式写入：永不失败 */
void rb_entry_write(RingBufferEntry *rb, const void *entry)
{
    if (rb->size == rb->capacity) {
        /* 满了：丢掉最旧（start 指针前移），size 不变 */
        rb->start = (rb->start + 1) % rb->capacity;
    } else {
        rb->size++;
    }

    int pos = idx(rb, rb->size - 1) * rb->entry_size;
    memcpy(rb->buffer + pos, entry, rb->entry_size);
}

/* 读 n 条；返回真正读出的条数 */
int rb_entry_read(RingBufferEntry *rb, void *dst, int n)
{
    if (n > rb->size) n = rb->size;
    for (int i = 0; i < n; i++) {
        int pos = idx(rb, i) * rb->entry_size;
        memcpy((char*)dst + i * rb->entry_size, rb->buffer + pos, rb->entry_size);
    }
    rb->start = (rb->start + n) % rb->capacity;
    rb->size  -= n;
    return n;
}
