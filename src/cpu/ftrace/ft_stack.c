#include "ft_stack.h"



// 初始化栈
void init_stack(Stack *s) {
    s->head = NULL;
}

// 判断栈是否为空
int is_empty(Stack *s) {
    return s->head == NULL;
}

// 压栈：新节点插到 head 前面
void push(Stack *s, char *name, uint32_t value, uint32_t ret) {
    StackNode *node = (StackNode *)malloc(sizeof(StackNode));
    strncpy(node->name, name, MAX_NAMENUM);
    node->name[MAX_NAMENUM-1] = '\0';
    node->value = value;
    node->ret = ret;
    node->next = s->head;
    s->head = node;

    printf("ADD :0x%08x %s\n", s->head->value, s->head->name);
}

// 弹栈：删除 head 节点
void pop(Stack *s) {
    if (is_empty(s)) return;
    printf("Delete :0x%08x %s\n", s->head->value, s->head->name);
    StackNode *temp = s->head;
    s->head = temp->next;
    free(temp);
}

// 查看栈顶元素（不弹出）
uint32_t peek(Stack *s) {
    if (is_empty(s)) return 0;
    return s->head->ret;
}

int i = 0;
// 🔁 从栈底遍历（递归方式）
void print_from_bottom(StackNode *node) {
    if (node == NULL) {
        i = 0;
        return;
    }
    print_from_bottom(node->next); // 先递归到底
    for (int j = 0; j < i; j++) {
        printf("  ");
    }
    printf("0x%08x: %s \n", node->value, node->name);     // 然后回溯打印
    i++;
}

void traverse_from_bottom(Stack *s) {
    print_from_bottom(s->head);
    printf("\n");
}


// 释放栈内存
void destroy_stack(Stack *s) {
    StackNode *curr = s->head;
    while (curr != NULL) {
        StackNode *temp = curr;
        curr = curr->next;
        free(temp);
    }
    s->head = NULL;
}
