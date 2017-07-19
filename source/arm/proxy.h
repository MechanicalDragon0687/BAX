#pragma once
#include <common.h>

#define IPC_BIN_SYMBOL (_binary_ipc_bin_start)
#define IPC_LEN_SYMBOL (_binary_ipc_bin_size)

#define IPC_BIN_LOC    ((const void*)&IPC_BIN_SYMBOL)
#define IPC_BIN_LEN    ((const unsigned int)&IPC_LEN_SYMBOL)

extern unsigned const char IPC_BIN_SYMBOL[];
extern unsigned const int  IPC_LEN_SYMBOL;

void proxy_boot(uint32_t addr);
void proxy_init(void);
