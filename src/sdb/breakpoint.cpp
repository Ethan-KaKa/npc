#include "sdb.h"

#define NR_BP 32

typedef struct breakpoint {
    int NO;   //序号
    vaddr_t addr;
    struct breakpoint *next;
} BP;

static BP bp_pool[NR_BP] = {};
static BP *bp_head = NULL, *bp_free = NULL;

void init_bp_pool() {
    for (int i = 0; i < NR_BP; i++) {
      bp_pool[i].NO = i;
      bp_pool[i].next = (i == NR_BP - 1 ? NULL : &bp_pool[i + 1]);
    }
    bp_head = NULL;
    bp_free = bp_pool;
  }
  
void new_bp(char *args) {
    if (bp_free == NULL) {
      printf("No free breakpoint slot!\n");
      return;
    }
  
    vaddr_t addr = strtoul(args, NULL, 16);  // 解析地址
  
    BP *new_bp = bp_free;
    bp_free = bp_free->next;
  
    new_bp->addr = addr;
    new_bp->next = bp_head;
    bp_head = new_bp;
  
    printf("Set breakpoint %d at 0x%08x\n", new_bp->NO, addr);
}

void free_bp(int id) {
    BP *prev = NULL, *cur = bp_head;
  
    while (cur != NULL) {
      if (cur->NO == id) {
        if (prev == NULL) bp_head = cur->next;
        else prev->next = cur->next;
  
        cur->next = bp_free;
        bp_free = cur;
  
        printf("Deleted breakpoint %d\n", id);
        return;
      }
      prev = cur;
      cur = cur->next;
    }
  
    printf("No breakpoint with id %d found.\n", id);
  }
  
void print_bp() {
    if (bp_head == NULL) {
      printf("No breakpoints set.\n");
      return;
    }
  
    BP *p = bp_head;
    while (p != NULL) {
      printf("Breakpoint %d at 0x%08x\n", p->NO, p->addr);
      p = p->next;
    }
}

int check_bp(vaddr_t pc) {
  BP *bp = bp_head;
  while (bp != NULL) {
    if (bp->addr == pc) {
      printf("Hit breakpoint %d at 0x%08x\n", bp->NO, pc);
      return 1;
    }
    bp = bp->next;
  }
  return 0;
}

