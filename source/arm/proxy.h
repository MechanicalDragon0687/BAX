#pragma once
#include <common.h>

#define IPC_BIN_SYMBOL (_binary_ipc_bin_start)
#define IPC_LEN_SYMBOL (_binary_ipc_bin_size)

#define IPC_BIN_LOC    ((const unsigned char*)&IPC_BIN_SYMBOL)
#define IPC_BIN_LEN    ((const unsigned int)&IPC_LEN_SYMBOL)

extern const unsigned char IPC_BIN_SYMBOL[];
extern const unsigned int  IPC_LEN_SYMBOL;

void proxy_boot(uint32_t addr);
void proxy_init(void);
