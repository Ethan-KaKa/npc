#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "common.h"

#define CONFIG_PC_RESET_OFFSET 0x0

#define PMEM_LEFT  ((paddr_t)CONFIG_MBASE)
#define PMEM_RIGHT ((paddr_t)CONFIG_MBASE + CONFIG_MSIZE - 1)
#define RESET_VECTOR (PMEM_LEFT + CONFIG_PC_RESET_OFFSET)

#define CONFIG_MSIZE 128 * 1024 * 1024 //总大小
#define PG_ALIGN __attribute((aligned(4096))) //页对齐
#define CONFIG_MBASE 0x80000000 //起始

/* convert the guest physical address in the guest program to host virtual address in NEMU */
uint8_t* guest_to_host(paddr_t paddr);
/* convert the host virtual address in NEMU to guest physical address in the guest program */
paddr_t host_to_guest(uint8_t *haddr);

void init_mem();

word_t paddr_read(paddr_t addr, int len);
void paddr_write(paddr_t addr, int len, word_t data);

long load_img(char *img_file);

#endif

