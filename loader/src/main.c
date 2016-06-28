void _start()
{
    // Dynamic size would be *so* much better...
    for (int i = 0; i < (1 << 19); i++) // Max payload size is 512kB
        *((char*)0x23F00000 + i) = *((char*)0x24F00000 + i);

        // From Gelex's libctr9_io: https://github.com/gemarcano/libctr9_io/blob/eba7cd859fbbbd1d827d1379484b7792fab7b575/src/ctr_system_ARM.c#L17
    	__asm volatile (
		"mov r1, #0 @segment \n\t"

		"1: \n\t" //outer_loop
			"mov r0, #0 @line \n\t"
			"2: \n\t" //inner_loop
				"orr r2, r1, r0 \n\t" //make r2 be the resulting combination of
				                      //segment and line

				"mcr p15, 0, r2, c7, c14, 2 \n\t" //Clean and flush the line

				"add r0, r0, #0x20 \n\t" //move line to next line

				//The number of lines depends on the cache size, 0x400 for the
				//3DS data cache since it has 8KB of cache. Refer to the ARM
				//documentation for more details.
				"cmp r0, #0x400 \n\t"
				"bne 2b \n\t" //inner_loop

			"add r1, r1, #0x40000000 \n\t"
			"cmp r1, #0x0 \n\t"
			"bne 1b \n\t" //outer_loop

		"mov r0, #0 \n\t"
		"mcr p15, 0, r0, c7, c5, 0 \n\t" //Flush instruction cache
		"mcr p15, 0, r0, c7, c10, 4 \n\t" //drain write buffer
		:::"r0","r1","r2","cc"
	);

    ((void (*)(void))0x23F00000)();
}
