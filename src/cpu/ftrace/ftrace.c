#include <ftrace.h>
#include <elf.h>
#include "ft_stack.h"

//* 栈 *//
// 名称存储和返回地址

#define MAX_NAMENUM 64
#define MAX_FUNCNUM 64
Stack s;

struct ft_info
{
    uint32_t value;
    char name[MAX_NAMENUM];
}ft_info[MAX_FUNCNUM];
int ft_index = 0;


static long load_elf(char *elf_file) {
    if (elf_file == NULL) {
      Assert(0 ,"No elf input. No ftrace");
      return 0;
    }
  
    FILE *fp = fopen(elf_file, "rb");
    Assert(fp, "Can ont open %s", elf_file);
  
    printf("The elf is %s", elf_file);
    

    int ret;

    //1.读取elf头
    Elf32_Ehdr ehdr;
    ret = fread(&ehdr, sizeof(ehdr), 1, fp);
    Assert(ret == 1, "can't read ehdr");

    //2.定位头表
    fseek(fp, ehdr.e_shoff, SEEK_SET);
    Elf32_Shdr shdrs[ehdr.e_shnum];
    ret = fread(shdrs, sizeof(Elf32_Shdr), ehdr.e_shnum, fp);
    Assert(ret == ehdr.e_shnum, "can't read shdr");

    //3.定位symtab + strtab
    Elf32_Shdr *symtab = NULL, *strtab = NULL;
    for (int i = 0; i < ehdr.e_shnum; i++) {
        if (shdrs[i].sh_type == SHT_SYMTAB) {
            symtab = &shdrs[i];
            Assert(symtab->sh_link < ehdr.e_shnum, "Invalid sh_link: %u", symtab->sh_link);
            strtab = &shdrs[symtab->sh_link]; // 使用 sh_link 选择 .strtab
            break;
        }
    }
    if (!symtab || !strtab) {
        Assert(0, "Can't find symtab or strtab");
        fclose(fp);
        return 0;
    }

    //4.
        //read strtab
    char *strtab_data = (char *)malloc(strtab->sh_size);
    fseek(fp, strtab->sh_offset, SEEK_SET);
    ret = fread(strtab_data, strtab->sh_size, 1, fp);
    Assert(ret == 1, "can't read strdata");

    printf("size is %d", strtab->sh_size);
        //read symtab
    int entry_num = symtab->sh_size / symtab->sh_entsize;
    Elf32_Sym sym;
    fseek(fp, symtab->sh_offset, SEEK_SET);
    for (int i = 0; i < entry_num; i++) {
        ret = fread(&sym, sizeof(sym), 1, fp);
        Assert(ret == 1, "can't read symtab");
        const char* name = strtab_data + sym.st_name;

        if (ELF32_ST_TYPE(sym.st_info) == 2) {
            strncpy(ft_info[ft_index].name, name, MAX_NAMENUM);
            ft_info[ft_index].value = sym.st_value;
            ft_index++;
        }
    }
    
    for (int i = 0; i < strtab->sh_size; i++) {
        if (strtab_data[i] == '\0') continue;
        while (i < strtab->sh_size && strtab_data[i] != '\0') i++;
    }

    free(strtab_data);
    fclose(fp);

    return 0;
}

void init_ftrace(char *elf_file) {
    load_elf(elf_file);

    init_stack(&s);
}

void check_ft_info(uint32_t value, uint32_t ret) {
    //检查是否在stack的ret中
    if (ret == peek(&s)) {
        pop(&s);
        return;
    }

    for (int i = 0; i < ft_index; i++) {
        if (ft_info[i].value == value) {
            push(&s, ft_info[i].name, value, ret);
            break;
        }
    }

}

void print_ft_info() {
    traverse_from_bottom(&s);
}

void exit_ftrace() {
    destroy_stack(&s);
}