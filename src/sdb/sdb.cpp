#include "../../include/sdb/sdb.h"
extern "C" {
  #include <readline/readline.h>
  #include <readline/history.h>
}

/* 声明函数 */
  //外部
void init_regex();
void print_ft_info();
  //内部
static int cmd_d(char *args);
static int cmd_w(char *args);
static int cmd_b(char *args);
static int cmd_p(char *args);
static int cmd_x(char *args);
static int cmd_info(char *args);
static int cmd_si(char *args);
static int cmd_c(char *args);
static int cmd_q(char *args);
static int cmd_help(char *args);
static int cmd_ft(char *args);

/* 辅助函数 */
static char* rl_gets() {
    static char *line_read = NULL;

    if (line_read) {
        free(line_read);
        line_read = NULL;
    }

    line_read = readline("(npc) ");

    if (line_read && *line_read) {
        add_history(line_read);
    }

    return line_read;
}


static struct {
    const char *name;
    const char *description;
    int (*handler) (char *);
  } cmd_table [] = {
    { "help", "Display information about all supported commands", cmd_help },
    { "c", "Continue the execution of the program", cmd_c },
    { "q", "Exit NEMU", cmd_q },
    { "si", "Execute one instruction", cmd_si },
    { "info", "info (r)/(w)/(b)", cmd_info },
    { "x", "x N EXPR", cmd_x },
    { "p", "p $expr_resource", cmd_p },
    { "w", "w EXPR", cmd_w },
    { "b", "b 0x...", cmd_b },
    { "d", "d (w)/(b) N", cmd_d},
    { "ft", "get ftrace", cmd_ft},
  
    /* TODO: Add more commands */
  
  };
  
#define NR_CMD ARRLEN(cmd_table)

/* 主函数 */
void sdb_mainloop() {
    for (char *str; (str = rl_gets()) != NULL; ) {
        char *str_end = str + strlen(str);
    
        /* extract the first token as the command */
        char *cmd = strtok(str, " ");
        if (cmd == NULL) { continue; }
    
        /* treat the remaining string as the arguments,
         * which may need further parsing
         */
        char *args = cmd + strlen(cmd) + 1;
        if (args >= str_end) {
          args = NULL;
        }

        int i;
        for (i = 0; i < NR_CMD; i ++) {
        if (strcmp(cmd, cmd_table[i].name) == 0) {
            if (cmd_table[i].handler(args) < 0) { return; }
            break;
        }
        }

        if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
    }
}

void init_sdb() {
    /* Compile the regular expressions. */
    init_regex();
  
    /* Initialize the watchpoint pool. */
    init_wp_pool();
  
    // Initialize the breakpoint
    init_bp_pool();
}

/* 命令实现 */
static int cmd_help(char *args) {
    /* extract the first argument */
    char *arg = strtok(NULL, " ");
    int i;
  
    if (arg == NULL) {
      /* no argument given */
      for (i = 0; i < NR_CMD; i ++) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
      }
    }
    else {
      for (i = 0; i < NR_CMD; i ++) {
        if (strcmp(arg, cmd_table[i].name) == 0) {
          printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
          return 0;
        }
      }
      printf("Unknown command '%s'\n", arg);
    }
    return 0;
}

static int cmd_c(char *args) {
    cpu_exec(-1);
    return 0;
}

static int cmd_q(char *args) {
    npc_state.state = NPC_QUIT;
    return -1;
}

static int cmd_si(char *args){
    int n = 1;
    
    if(args) { n = atoi(args); }
    
    cpu_exec(n);
    
    return 0;
}

static int cmd_info(char *args){
    if (!args) { printf("Please input the mode"); }

    if (*args == 'r') {
      printf("display the info of reg\n");
      print_regs();
    }
    else if (*args == 'w') {
      print_wp();
    }
    else if (*args == 'b') {
      print_bp();
    }
    else { printf("We don't have this mode"); }

    return 0;
}

static int cmd_x(char *args){
    char *N = strtok(args, " ");
    char *expr = N + strlen(N) + 1;

    int n = atoi(N);
    uint32_t Posi = strtoul(expr, NULL, 0);    

    for (int i = 0; i < n; i++){
        uint32_t val = paddr_read(Posi + i*4, 4);
        printf("0x%08x: 0x%08x\n", (Posi + i*4), val);
    }
    return 0;   
}

static int cmd_p(char *args){
    char *filename = args;

    char line[100];
    char expr_read[33];
    uint32_t result_ri;
    uint32_t result;

    int posi = 1;//当前读取到的行

    bool *success = (bool *)malloc(sizeof(bool));
    
    FILE *fp = fopen(filename, "r");

    bool right = 1;

    while (fgets(line, sizeof(line), fp)) {

        sscanf(line, "%d %[^\n]", &result_ri, expr_read);

        result = expr(expr_read, success);

        if (*success != false && result_ri != result) {
            right = 0;
            printf("%s : read result:%u || result:%u \n", expr_read, result_ri, result);
        }        
        posi++;
    }

    if (right) {
      printf("ALL test past\n");
    }
    
    fclose(fp);

    return 0;
}

static int cmd_b(char *args) {
    new_bp(args);
  
    return 0;
}

static int cmd_d(char *args) {
    char *mode = strtok(args, " ");
    int num = atoi(mode + strlen(mode) + 1);
  
    if (*mode == 'w') {
      free_wp(num);
    }
    else if (*mode == 'b') {
      free_bp(num);
    }
    else printf("We don't have this kind of type\n");
  
    return 0;
}
  
static int cmd_w(char *args) {
    new_wp(args);
  
    return 0;
}

static int cmd_ft(char *args) {
    print_ft_info();
    return 0;
}