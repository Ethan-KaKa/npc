// bstrlib.c
#include "bstrlib.h"
#include <stdlib.h>
#include <string.h>

/* 创建一个 bstring 对象（从内存块 blk 中复制 len 字节） */
bstring blk2bstr(const void *blk, int len) {
    if (blk == NULL || len < 0) return NULL;

    bstring b = (bstring) malloc(sizeof(struct tagbstring));
    if (b == NULL) return NULL;

    b->mlen = len + 16;  // 预留空间，防止频繁 realloc
    b->slen = len;
    b->data = (unsigned char *) malloc(b->mlen);
    if (b->data == NULL) {
        free(b);
        return NULL;
    }

    if (len > 0) {
        memcpy(b->data, blk, len);
    }

    return b;
}

/* 返回 bstring 对象的长度 */
int blength(const bstring b) {
    if (b == NULL) return -1;
    return b->slen;
}

/* 释放 bstring 对象 */
int bdestroy(bstring b) {
    if (b == NULL) return -1;
    if (b->data != NULL) free(b->data);
    free(b);
    return 0;
}
