	.global arch_core_start
	.extern arch_start_first_task

	/* global variables */
	.global arch_core_sp
	.global arch_task_sp
	.global arch_core_ivpr
	.global arch_core_r13

	.section .sdata

	.align 4
arch_core_sp:
	.long 0
arch_task_sp:
	.long 0
arch_core_ivpr:
	.long 0
arch_core_r13:
	.long 0
	
	.section .text

	.align 4
	.type arch_core_start, %function
arch_core_start:
core_e200z7_start:
	wrteei 0 /* disable interrupts */

	/* MMU */
	e_lis r3, 0x1001
	mtmas0 r3
	e_lis r4, 0xc000
	e_or2i r4, 0x0480
	mtmas1 r4
	e_lis r5, 0x4000
	e_or2i r5, 0x0028
	mtmas2 r5
	e_lis r6, 0x4000
	e_or2i r6, 0x003f
	mtmas3 r6
	tlbld r0
	e_lis r3, 0x1000
	mtmas0 r3
	e_lis r4, 0xc000
	e_or2i r4, 0x0700
	mtmas1 r4
	e_lis r5, 0x0000
	e_or2i r5, 0x0020
	mtmas2 r5
	e_lis r6,0x0000
	e_or2i r6, 0x003f
	mtmas3 r6
	tlbld r0
	e_lis r3, 0x1002
	mtmas0 r3
	e_lis r4, 0xc000
	e_or2i r4, 0x0580
	mtmas1 r4
	e_lis r5, 0xffe0
	e_or2i r5, 0x000a
	mtmas2 r5
	e_lis r6, 0xffe0
	e_or2i r6, 0x003f
	mtmas3 r6
	tlbld r0
	e_lis r3, 0x1003
	mtmas0 r3
	e_lis r4, 0xc000
	e_or2i r4, 0x0700
	mtmas1 r4
	e_lis r5, 0x2000
	e_or2i r5, 0x0020
	mtmas2 r5
	e_lis  r6,0x0000
	e_or2i r6, 0x003f
	mtmas3 r6
	tlbld r0
	e_lis r3, 0x1004
	mtmas0 r3
	e_lis r4, 0xc000
	e_or2i r4, 0x0580
	mtmas1 r4
	e_lis r5, 0xc3e0
	e_or2i r5, 0x000a
	mtmas2 r5
	e_lis r6, 0xc3e0
	e_or2i r6, 0x003f
	mtmas3 r6
	tlbld r0

	/* turn off watchdog */
	e_li r3, 0
	mttcr r3

	/* turn on branch prediction */
	e_li r3, 0x201
	mtbucsr r3
	se_isync

	/* enable MSR[ME] */
	e_li r31, 0x1000
	mtmsr r31

	/* set IVPR */
	e_lis r3, arch_core_ivpr@ha
	e_lwz r3, arch_core_ivpr@l (r3)
	mtivpr r3

	/* set IVORs, as they are not set automatically on z7 */
	e_rlwinm r3, r3, 0, 16, 27
	mtspr 400, r3    /* IVOR0 400 Critical input */
	e_add16i r0, r3, 0x10
	mtspr 401, r0   /* IVOR1 401 Machine check */
	e_add16i r0, r3, 0x20
	mtspr 402, r0   /* IVOR2 402 Data storage */
	e_add16i r0, r3, 0x30
	mtspr 403, r0   /* IVOR3 403 Instruction storage */
	e_add16i r0, r3, 0x40
	mtspr 404, r0   /* IVOR4 404 External input */
	e_add16i r0, r3, 0x50
	mtspr 405, r0   /* IVOR5 405 Alignment */
	e_add16i r0, r3, 0x60
	mtspr 406, r0   /* IVOR7 406 Program */
	e_add16i r0, r3, 0x70
	mtspr 407, r0   /* IVOR7 407 Floating-point unavailable */
	e_add16i r0, r3, 0x80
	mtspr 408, r0   /* IVOR8 408 System call */
	e_add16i r0, r3, 0x90
	mtspr 409, r0   /* IVOR9 409 Auxiliary processor unavailable */
	e_add16i r0, r3, 0xa0
	mtspr 410, r0   /* IVOR10 410 Decrementer */
	e_add16i r0, r3, 0xb0
	mtspr 411, r0   /* IVOR11 411 Fixed-interval timer interrupt */
	e_add16i r0, r3, 0xc0
	mtspr 412, r0   /* IVOR12 412 Watchdog timer interrupt */
	e_add16i r0, r3, 0xd0
	mtspr 413, r0   /* IVOR13 413 Data TLB error */
	e_add16i r0, r3, 0xe0
	mtspr 414, r0   /* IVOR14 414 Instruction TLB error */
	e_add16i r0, r3, 0xf0
	mtspr 415, r0   /* IVOR15 415 Debug */
	/* IVOR16-IVOR31 - Reserved for future architectural use */
	e_add16i r0, r3, 0x100
	mtspr 528, r0  /* IVOR32 528 SPE APU unavailable */
	e_add16i r0, r3, 0x110
	mtspr 529, r0  /* IVOR33 529 SPE floating-point data exception */
	e_add16i r0, r3, 0x120
	mtspr 530, r0  /* IVOR34 530 SPE floating-point round exception */
	e_add16i r0, r3, 0x130
	mtspr 531, r0  /* IVOR35 531 Performance Monitor Interrupt */
	
	/* set r1 / main stack */
	e_lis r1, arch_core_sp@ha
	e_lwz r1, arch_core_sp@l (r1)

	/* set r13 to bss */
	e_lis r13, arch_core_r13@ha
	e_lwz r13, arch_core_r13@l (r13)

	/* set r3 to task stack */
	e_lis r3, arch_task_sp@ha
	e_lwz r3, arch_task_sp@l (r3)

	e_bl arch_start_first_task
