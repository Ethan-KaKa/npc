#include "registers.h"
#define REG_NUM 16

const char *regs[] = {
    "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5"
} ;

word_t regs_value[REG_NUM];

//从verilog获取
void get_regs(uint32_t *rf_ptr) {
    for (int i = 0; i < REG_NUM; i++) {
        regs_value[i] = rf_ptr[i];
    }
}

//给出
word_t get_reg(char *name) {
    for (int i = 0; i < REG_NUM; i++) {
        if (!strcmp(regs[i], name)) {
            return regs_value[i];
        }
    }

    printf("NO this reg\n");
    return 0;
}

void get_32reg(uint32_t *rf_ptr) {
    for (int i = 0; i < REG_NUM; i++) {
        rf_ptr[i] = regs_value[i];
    }
}

//diff
bool diff_checkregs(uint32_t *rf_ptr) {
    for (int i = 0; i < REG_NUM; i++) {
        if (regs_value[i] != rf_ptr[i]) return false;
    }
    return true;
}

//all
void print_regs() {
    int line_i = 0;
    const int line = 4;  //每行个数
    printf("npc_regs\n");
    for (int i = 0; i < REG_NUM; i++) {
        if (line_i == line) {
            printf("\n");
            line_i = 0;
        } 
        printf("%-3s 0x%.8x ", regs[i], regs_value[i]);
        line_i++;
    }
    printf("\n0x%.8x\n", get_pc());
}

void print_reg(char *name) {
    bool exist = false;
    for (int i = 0; i < REG_NUM; i++) {
        if (!strcmp(regs[i], name)) {
            printf("%s 0x%.8x\n", regs[i], regs_value[i]);
            exist = true;
            break;
        }
    }

    if (!exist) {
        printf("NO this reg\n");
    }
}

