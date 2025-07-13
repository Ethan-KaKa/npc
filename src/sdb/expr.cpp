#include "../../include/sdb/sdb.h"
#include <regex.h>

enum {
    TK_NOTYPE = 256,
    //运算符最低
    TK_EQ = 257,
    TK_NEQ = 258,
    TK_AND = 259,
    TK_SUM = 260,
    TK_DIFF = 261,
    TK_DIVI = 262, TK_MU = 263,
    TK_DEREF = 264, //指针
    //运算符最高
    TK_LE = 265,
    TK_RI = 266,
    TK_NUM = 267,
    TK_HEXNUM = 268,
    TK_REG = 269,
    

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", TK_SUM},         // plus
  {"-", TK_DIFF},
  {"\\*", TK_MU},
  {"/", TK_DIVI},
  {"\\(", TK_LE},
  {"\\)", TK_RI},
  //need str
  {"0[xX][0-9a-fA-F]+", TK_HEXNUM},    //0x number
  {"[0-9]+[uU]?", TK_NUM},   //7 need str
  {"\\$[a-zA-Z_][a-zA-Z0-9_]*", TK_REG},
  //need str
  {"==", TK_EQ},        // equal
  {"!=", TK_NEQ},
  {"&&", TK_AND},
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static Token init_tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

int check_parentheses(int p, int q) {
    int kuo[33];  // 对应tokens中的32位
    memset(kuo, -1, sizeof(kuo));

    for (int i = q; i >= p; i--) {  // 倒序找‘（’
        if (tokens[i].type == TK_LE) {
            for (int j = i; j <= q; j++) {
                if (tokens[j].type == TK_RI && kuo[j] == -1) {
                    kuo[j] = i;
                    kuo[i] = 1;  // ‘（’匹配到了
                    break;
                }
            }
            if (kuo[i] == -1) return 2;
        }
    }

    for (int i = p; i <= q; i++) {
        if (tokens[i].type == TK_RI && kuo[i] == -1) return 2;
    }

    // 三种情况 0-括号匹配 1-可运算 2-不可运算
    if (kuo[q] == p) return 0;
    else return 1;
}



//递归计算
uint32_t eval(int p, int q){
    if (p > q) {
        assert(0); //not normal
    }
    else if (p == q) {
      if (tokens[p].type == TK_REG) {
        uint32_t reg_value;
        bool* success = (bool*)malloc(sizeof(bool));
        reg_value = get_reg(tokens[p].str);
        free(success);
        return reg_value;
      }
      else {
        return (uint32_t)strtoul(tokens[p].str, NULL, 0);
      }
    }
    else if (check_parentheses(p, q) == 0) {
        return eval(p + 1, q - 1);
    }
    else {
      int op = 0, op_type = 265;
      for (int i = p; i <= q; i++)//寻找主运算符
      {
        if (tokens[i].type >= 257 &&
            tokens[i].type <= 264 &&
            check_parentheses(p, i - 1) != 2 &&
            check_parentheses(i + 1, q) != 2 &&
            ( op_type >= tokens[i].type                         /* 原先的规则       */
              || (op_type == TK_DIVI && tokens[i].type == TK_MU)/*  / 之后遇到 *  */ )
            )//+-*/ && 不在括号内 && 优先级高
        {
          op = i;
          op_type = tokens[i].type;
        }
      }

      uint32_t val1 = 0;
      if (op_type != 264) { //指针不用算前面
        val1 = eval(p, op - 1);
      }
      uint32_t val2 = eval(op + 1, q);

      switch (op_type) {
        case 257: return val1 == val2;
        case 258: return val1 != val2;
        case 259: return val1 && val2;
        case 260: return val1 + val2;
        case 261: return val1 - val2;
        case 262: return val1 / val2;
        case 263: return val1 * val2;
        case 264: {
          uint32_t Posi = val2;
          return paddr_read(Posi, 4);
        }
        default: assert(0);
      }
    }
}

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  //初始化
  memcpy(tokens, init_tokens, sizeof(tokens));
  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        //调试信息
        /* Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        */

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        tokens[nr_token].type = rules[i].token_type;
        if (i >= 7 && i <= 9) { 
            if (substr_len <= 32) {
                if (substr_start[substr_len-1] == 'u' || substr_start[substr_len-1] == 'U') {
                  substr_len--;
                } 
                strncpy(tokens[nr_token].str, substr_start, substr_len);       
            }
            else {
                assert(0);  //add what?
            }
        }
        nr_token++;

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}


word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  else {
    *success = true;
  }

  /* TODO: Insert codes to evaluate the expression. */

  //删去空格 是否会影响之后的识别？
  for (int i = 0; i < nr_token; i++) {
    if (tokens[i].type == 256) {
      for (int j = i; j < nr_token; j++) {
        tokens[j] = tokens[j + 1];
      }
      nr_token--;
    }
  }

  //指针解引用
  for (int i = 0; i < nr_token; i++) {
    if (tokens[i].type == TK_MU &&
      ( i == 0 || 
        tokens[i - 1].type == TK_LE ||
        (tokens[i - 1].type >= TK_EQ && tokens[i - 1].type <= TK_MU)
      )
    ) {
      tokens[i].type = TK_DEREF;
    }
  }

    /* 计算tokens 
     * type：类型 str：数字
     * 起始：0 结尾：nr_token
     */
    return eval(0, nr_token - 1);
}
