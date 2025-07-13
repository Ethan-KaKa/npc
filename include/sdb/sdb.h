#ifndef __SDB_H__
#define __SDB_H__

#include "../common.h"
#include "../debug.h"
#include "../cpu_exec.h"

void init_sdb();
void sdb_mainloop();

word_t expr(char *e, bool *success);

//watchpoint的
void init_wp_pool();
void new_wp(char *expr_in);
void free_wp(int num);
void print_wp();
int check_wp();

//breakpoint
void init_bp_pool();
void new_bp(char *args);
void free_bp(int id);
void print_bp();
int check_bp(vaddr_t pc);

#endif