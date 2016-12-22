@ chainload.s

@ void chainload(const void *payload, const size_t len)

@ uses memcpy256 @ 0xFFFF03F0
@ void memcpy256(void *src, void *ddest, const size_t size)

.thumb
.type chainload, %function
.global chainload
chainload:
	mov r2, r1 @ move size to the correct register
	ldr r1, =_main_
	mov lr, r1 @ set up the destination + a fake return address
	ldr r3, =0xFFFF03F0
	bx r3
