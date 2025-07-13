#ifndef BSTRLIB_H
#define BSTRLIB_H

#ifdef __cplusplus
extern "C" {
#endif

/* bstring 类型定义：这里用一个结构体来保存字符串相关信息，
 * mlen：分配的内存大小，
 * slen：当前字符串的长度，
 * data：指向实际字符数据的指针。 */
typedef struct tagbstring {
    int mlen;               /* 分配的内存长度 */
    int slen;               /* 字符串实际长度 */
    unsigned char *data;    /* 字符数据 */
} *bstring;

/* 将一块内存转换成 bstring 类型。参数 blk 是内存块指针，len 是内存块长度。
 * 该函数会分配一个新的 bstring，并复制 blk 中的数据。 */
bstring blk2bstr(const void *blk, int len);

/* 返回 bstring 内存中保存的字符数据指针，
 * 实际上可能只是返回 bstring->data。 */
unsigned char *bdata(const bstring s);

/* 返回 bstring 的字符串长度，即 bstring->slen。 */
int blength(const bstring s);

#ifdef __cplusplus
}
#endif

#endif
