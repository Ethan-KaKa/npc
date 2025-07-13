#include <dlfcn.h>

#include "cpu_exec.h"
#include "difftest-def.h"
bool diff_checkregs(uint32_t *rf_ptr);

void (*ref_difftest_memcpy)(paddr_t addr, void *buf, size_t n, bool direction) = NULL;
void (*ref_difftest_regcpy)(uint32_t *pc, void *dut, bool direction) = NULL;
void (*ref_difftest_exec)(uint64_t n) = NULL;
void (*ref_difftest_raise_intr)(uint64_t NO) = NULL;

void init_difftest(char *ref_so_file, long img_size, int port) {
    assert(ref_so_file != NULL);

    void *handle;
    handle = dlopen(ref_so_file, RTLD_LAZY);
    assert(handle);

    ref_difftest_memcpy = (void (*)(paddr_t, void*, size_t, bool))dlsym(handle, "difftest_memcpy");
    assert(ref_difftest_memcpy);

    ref_difftest_regcpy = (void (*)(uint32_t*, void*, bool))dlsym(handle, "difftest_regcpy");
    assert(ref_difftest_regcpy);

    ref_difftest_exec = (void (*)(uint64_t))dlsym(handle, "difftest_exec");
    assert(ref_difftest_exec);

    ref_difftest_raise_intr = (void (*)(uint64_t))dlsym(handle, "difftest_raise_intr");
    assert(ref_difftest_raise_intr);

    void (*ref_difftest_init)(int) = (void (*)(int))dlsym(handle, "difftest_init");
    assert(ref_difftest_init);

    Log("Differential testing: %s", ANSI_FMT("ON", ANSI_FG_GREEN));
    Log("The result of every instruction will be compared with %s. "
        "This will help you a lot for debugging, but also significantly reduce the performance. "
        "If it is not necessary, you can turn it off in menuconfig.", ref_so_file);

    ref_difftest_init(port);
    ref_difftest_memcpy(RESET_VECTOR, guest_to_host(RESET_VECTOR), img_size, DIFFTEST_TO_REF);
    // ref_difftest_regcpy(&cpu, DIFFTEST_TO_REF);
}

const char *ref_regname[] = {
    "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5"
} ;

static void checkregs(uint32_t *ref_reg, vaddr_t pc) {
    if (!diff_checkregs(ref_reg)) {
        npc_state.state = NPC_ABORT;
        npc_state.halt_pc = pc;
        print_regs();

        int line_i = 0;
        const int line = 4;  //每行个数 
        printf("ref_regs\n");
        for (int i = 0; i < 16; i++) {
            if (line_i == line) {
                printf("\n");
                line_i = 0;
            } 
            printf("%-3s 0x%.8x ", ref_regname[i], ref_reg[i]);
            line_i++;
        }
    }
}

void difftest_step(vaddr_t dut_pc, vaddr_t dut_npc) {
    vaddr_t ref_npc;
    uint32_t ref_reg[16];

    
    ref_difftest_regcpy(&ref_npc, ref_reg, DIFFTEST_TO_DUT);
    printf("dut_pc:0x%.8x (ref_npc:0x%.8x ≠ dut_npc:0x%.8x)\n", dut_pc, ref_npc, dut_npc);
    if (ref_npc == dut_pc) {
        checkregs(ref_reg, dut_pc);
    }
    else {
        Assert(0, "dut_pc:0x%.8x (ref_npc:0x%.8x ≠ dut_npc:0x%.8x)", dut_pc, ref_npc, dut_npc);
    }

    ref_difftest_exec(1);
    return;
}