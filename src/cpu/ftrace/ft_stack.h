#ifndef __FT_STACK_H__
#define __FT_STACK_H__

#include <common.h>

#define MAX_NAMENUM 64
// 栈节点定义
typedef struct StackNode {
    char name[MAX_NAMENUM];
    uint32_t value;
    uint32_t ret;
    struct StackNode *next;
} StackNode;

// 栈结构（使用 head 作为栈顶）
typedef struct {
    StackNode *head;
} Stack;

void init_stack(Stack *s);

void push(Stack *s, char *name, uint32_t value, uint32_t ret);

void pop(Stack *s);

uint32_t peek(Stack *s);

void traverse_from_bottom(Stack *s);

void destroy_stack(Stack *s);

#endif