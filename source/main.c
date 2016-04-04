#include "main.h"

#include "types.h"
#include "draw.h"
#include "memory.h"
#include "fs.h"
#include "chain.h"

void load_and_run() {
	memcpy((void*)PAYLOAD_ADDR, chain_bin, chain_bin_len);
	((void(*)(void))PAYLOAD_ADDR)();
}

int main(){
	mountSDMC();
    u16 pressed = HID_PAD;
	if ((*(vu8 *)0x10010000 == 0) & !(pressed & BUTTON_R1)) // check if this is a coldboot and if key is pressed
		animationLoop();
	load_and_run();
	return 0;
}
