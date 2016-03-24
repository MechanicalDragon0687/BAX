#include "types.h"
#include "draw.h"
#include "memory.h"

#define PAYLOAD_ADDR	0x24F00000

#include "fs.h"
#include "chain.h"

/*const char *payloads[] = {"arm9loaderhax/arm9bootloader.bin", "arm9bootloader.bin", "aurei/arm9payload.bin", "rei/arm9payload.bin", "/NANDUnBricker.bin"};

void load_and_run() {
	for (u16 i = 0; i <= sizeof(*payloads); i++)
		if (fileExists(payloads[i])) {
			fileRead((u8*)PAYLOAD_ADDR, payloads[i], fileSize(payloads[i]));
			((void(*)(void))PAYLOAD_ADDR)();
		}
}*/

void load_and_run() {
	memcpy((void*)PAYLOAD_ADDR, chain_bin, chain_bin_len);
	((void(*)(void))PAYLOAD_ADDR)();
}

int main(){
	mountSDMC();
	if (*(vu8 *)0x10010000 == 0) // check if this is a coldboot
		drawBootScreen();
	load_and_run();
	return 0;
}
