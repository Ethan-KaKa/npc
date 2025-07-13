#ifndef __DIFFTEST_DEF_H__
#define __DIFFTEST_DEF_H__

#include "macro.h"
#include "common.h"

#define __EXPORT __attribute__((visibility("default")))
enum { DIFFTEST_TO_DUT, DIFFTEST_TO_REF };

#define diFfTEST_REG_SIZE (sizeof(uint32_t) * 16)


#endif