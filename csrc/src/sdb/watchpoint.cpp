#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
    //wp中不变的
  int NO;   //序号
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
    //需要改变的

    char wp_expr[32];
    uint32_t wp_value;
} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;//head使用中  free空闲

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

void new_wp (char *expr_in) {
    if (free_->next == NULL) {
        printf("Failed: The watchpoint is full");
        return;
    }

    WP* nwp = free_;

    strcpy(nwp->wp_expr, expr_in);
    
    //初始化求值
    bool* success = (bool*)malloc(sizeof(bool));
    nwp->wp_value = expr(expr_in, success);
    if (*success == false) {
        printf("This expr is wrong\n");
    }

    free(success);


    free_ = free_->next;
    if (head == NULL) {
        head = nwp;
    }

    printf("NEW_WP : %d %s %u\n", nwp->NO, nwp->wp_expr, nwp->wp_value);
}

void free_wp (int num) {
    for (int i = num; i < free_->NO; i++) {
        //需要改的往前移
        strcpy(wp_pool[i].wp_expr, wp_pool[i + 1].wp_expr);
        wp_pool[i].wp_value = wp_pool[i + 1].wp_value;
    }
    free_ = wp_pool + free_->NO - 1;
}

void print_wp () {
    WP* wp_i;

    if (head == NULL) {
        printf("There is no wp\n");
        return;
    }

    wp_i = head;

    while (wp_i->NO != free_->NO) {
        printf("%d %s %u\n", wp_i->NO, wp_i->wp_expr, wp_i->wp_value);

        wp_i = wp_i->next;
    }
}

//检查：0不发生变化 x发生变化的数量
int check_wp () {
    if (head == NULL) return 0;

    WP* wp_i = head;
    int return_value = 0;

    bool *success = (bool*)malloc(sizeof(bool));

    while (wp_i->NO != free_->NO) {
        uint32_t new_value = expr(wp_i->wp_expr, success);
        if (new_value != wp_i->wp_value) {
            printf("%d %s  old: 0x%08x new: 0x%08x \n", wp_i->NO, wp_i->wp_expr, wp_i->wp_value, new_value);

            wp_i->wp_value = new_value;
            return_value++;
        }

        wp_i = wp_i->next;
    } 

    free(success);

    return return_value;
}
