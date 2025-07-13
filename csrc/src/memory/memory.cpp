#include "../../include/memory.h"


//memory
static uint8_t pmem[CONFIG_MSIZE] PG_ALIGN = {};

uint8_t* guest_to_host(paddr_t paddr) { return pmem + paddr - CONFIG_MBASE; }
paddr_t host_to_guest(uint8_t *haddr) { return haddr - pmem + CONFIG_MBASE; }

static word_t pmem_read(paddr_t addr, int len) {
    uint8_t *host_addr = guest_to_host(addr);
    switch (len) {
        case 1: return *(uint8_t  *)host_addr;
        case 2: return *(uint16_t *)host_addr;
        case 4: return *(uint32_t *)host_addr;
        default: Assert(0, "date_len not in 1/2/4");
    }
}

static void pmem_write(paddr_t addr, int len, word_t data) {
    uint8_t *host_addr = guest_to_host(addr);
    switch (len) {
        case 1:*(uint8_t *)host_addr = data; break;
        case 2:*(uint16_t *)host_addr = data; break;
        case 4:*(uint32_t *)host_addr = data; break;
        default: Assert(0, "data_len not in 1/2/4");
    }
}

static void out_of_bound(paddr_t addr) {
    //????
}

void init_mem() {
    memset(pmem, 0, CONFIG_MSIZE);
}

bool in_pmem(paddr_t addr) {
    return addr - CONFIG_MBASE < CONFIG_MSIZE;
}


word_t paddr_read(paddr_t addr, int len) {
    if(in_pmem(addr)) return pmem_read(addr, len);
    out_of_bound(addr);
    return 0; 
}

void paddr_write(paddr_t addr, int len, word_t data) {
    if(in_pmem(addr)) {
        pmem_write(addr, len, data);
        return ;
    }
    out_of_bound(addr);
}


//加载img
long load_img(char *img_file) {
    if (img_file == NULL) Assert(0, "No input img_file");
  
    FILE *fp = fopen(img_file, "rb");
    Assert(fp, "Can not open '%s'", img_file);

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
  
    printf("The image is %s, size = %ld\n", img_file, size);
  
    fseek(fp, 0, SEEK_SET);
    int ret = fread(guest_to_host(RESET_VECTOR), size, 1, fp);
    assert(ret == 1);
  
    fclose(fp);
    return size;
  }