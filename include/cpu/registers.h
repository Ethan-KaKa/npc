#ifndef __REGISTERS_H__
#define __REGISTERS_H__

#include "../include/common.h"
#include "svdpi.h"
#include "cpu_exec.h"

void get_regs(uint32_t *rf_ptr);
void print_regs();
void print_reg(char *name);
word_t get_reg(char *name);


#endif