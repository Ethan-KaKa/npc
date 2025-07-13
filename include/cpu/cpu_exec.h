#ifndef __CPU_EXEC__
#define __CPU_EXEC__

#pragma message(">>> cpu_exec.h included")


#include "Vtao_top.h"
#include "Vtao_top___024root.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "common.h"
#include "memory.h"
#include "registers.h"
#include "sdb/sdb.h"
#include "sdb/trace.h"

void init_mainloop(char *img_file);
void cpu_exec(uint64_t n);
void exit_mainloop();
vaddr_t get_pc();

#endif