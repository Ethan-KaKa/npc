#include "cpu_exec.h"
#include "iritrace.h"
#include "difftest-def.h"

#define MAX_INST_TO_PRINT 10
#define REG_NUM 16

static bool g_print_step = false;
uint32_t pc;//保存当前pc
int cycle = 0; //记录进行到哪一个cycle

/* 外部函数 */
void init_difftest(char *ref_so_file, long img_size, int port);

/* 运行和波形记录需要的函数 */
extern "C" void ebreak_funct(int ebreak) {
    if (ebreak == 1) {
        npc_state.state = NPC_END;
    }
}

// 程序代码
VerilatedContext* contextp = NULL;
VerilatedVcdC* tfp = NULL;

static Vtao_top* top;

void step_and_dump_wave(){
    top->clk = 0; top->eval();
    contextp->timeInc(1);
    tfp->dump(contextp->time());

    top->clk = 1; top->eval();
    contextp->timeInc(1);
    tfp->dump(contextp->time());
}

void sim_init(){
    contextp = new VerilatedContext;
    tfp = new VerilatedVcdC;
    top = new Vtao_top;
    contextp->traceEverOn(true);
    top->trace(tfp, 0);
    tfp->open("dump.vcd");
}

void reset(){
    top->rst = 1;
    top->clk = 0; top->eval();
    top->clk = 1; top->eval();
    step_and_dump_wave();
    top->rst = 0;
}

void sim_exit(){
    step_and_dump_wave();
    tfp->close();

    delete top;
    delete contextp;
    delete tfp;
}

//获取信息

vaddr_t get_pc() {
    return pc;
}



/*  */
    //所需的函数
void check_ft_info(uint32_t value, uint32_t ret);
void get_32reg(uint32_t *rf_ptr);
void difftest_step(vaddr_t dut_pc, vaddr_t dut_npc);
static void trace_and_differest() {
    char buf[128];
    disassemble(buf, sizeof(buf), (uint64_t)pc, (uint8_t *)&top->inst, 4);
    //打印？
    if (g_print_step) {
        printf("0x%08x: %s\n", pc, buf);
    }

    //diff
    difftest_step(pc, top->npc);

    //watchpoint
    int check_num = check_wp();
    if (check_num != 0) {
        npc_state.state = NPC_STOP;
        printf("nextPC : 0x%08x\n", top->npc);
        printf("WP is triggered by %d diff\n", check_num);
    }

    //breakpoint
    if (check_bp(top->npc)) {
        npc_state.state = NPC_STOP;
      }

    //iritrace
    uint32_t regs[REG_NUM] = {0};
    get_32reg(regs);
    iritrace_record(pc, buf, cycle, regs);
    
    //ftrace
    check_ft_info(top->dnpc, top->snpc);  //跳转指令和返回指令
}


void init_mainloop(char *img_file) {
    sim_init();

    reset();

    if (!img_file) {
        Assert(0, "No input img_file");
    }

    //memory
    init_mem();

    long img_size = load_img(img_file);

    npc_state.state = NPC_STOP; //初始化为stop

    //trace
    init_disasm();

    //diff
    const char *diff_so_file = "/home/kaka/ysyx-workbench/nemu/build/riscv32-nemu-interpreter-so";
    init_difftest((char *)diff_so_file, img_size, 1234);
}

int exec_once() {
    //获取当前 PC
    pc = top->npc;

    //读取当前指令
    top->inst = paddr_read(pc, 4);

    //寄存器
    uint32_t *rf_ptr = (uint32_t*) &(top->rootp->tao_top__DOT__core_tao_exu__DOT__Reg_16__DOT__rf);
    get_regs(rf_ptr);

    //一个时钟周期
    step_and_dump_wave();

    //增加cycle
    cycle++;

    return 0;
}

void execute(uint64_t n) {
    for (;n > 0; n--) {
        exec_once();
        trace_and_differest();
        if (npc_state.state != NPC_RUNNING) break;
    }
}

/* 待更改
    1.完善abort情况
    2.测试irtrace可以正常运行
*/
void cpu_exec(uint64_t n) {
    g_print_step = (n < MAX_INST_TO_PRINT);
    switch (npc_state.state) {
        case NPC_END: case NPC_QUIT:
            printf("Program execution has ended. To restart the program, exit NPC and run again.\n");
            return;
        default: npc_state.state = NPC_RUNNING;
    }
  
    execute(n);
  
    switch (npc_state.state) {
        case NPC_RUNNING: npc_state.state = NPC_STOP; break;

        case NPC_END:
        case NPC_ABORT:
            Log("npc: %s at pc = 0x%.8x",
                (npc_state.state == NPC_ABORT ? ANSI_FMT("ABORT", ANSI_FG_RED) :
                (npc_state.halt_ret == 0 ? ANSI_FMT("HIT GOOD TRAP", ANSI_FG_GREEN) :
                    ANSI_FMT("HIT BAD TRAP", ANSI_FG_RED))),
                npc_state.halt_pc);
            // fall through
            if (npc_state.halt_ret != 0) {
                // HIT BAD TRAP → 打印 iritrace
                iritrace_print();
                iritrace_dump("iritrace_on_badtrap.bin");
            } 
        case NPC_QUIT: break;
    }

    
}


void exit_mainloop() {
    sim_exit();
}
