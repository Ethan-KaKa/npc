#include "common.h"
#include "debug.h"
#include "cpu_exec.h"

//初始化
//处理参数 把img_file传出去
//启动sdb

void init_ftrace(char *elf_file);
void init_iritrace(int num_entries);


void exit_ftrace();
void iritrace_destroy(void);

int main(int argc, char *argv[]) {
    //参数处理（暂时放在这）

    if(argc < 1){
        Assert(0, "NO img_file input");
    }
    if(argc < 2){
        Assert(0, "NO elf_file input");
    }

    //init
        //ft
    char *elf_file = argv[2];
    init_ftrace(elf_file);
        //ir
    init_iritrace(16);
        //
    char *img_file = argv[1];
    init_mainloop(img_file);
        //diff 见cpu_exec


    //启动sdb
    init_sdb();
    sdb_mainloop();


    //结束
    exit_ftrace();
    iritrace_destroy();
    
    exit_mainloop();
}