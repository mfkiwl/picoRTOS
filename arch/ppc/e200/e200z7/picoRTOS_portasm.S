	#include "picoRTOSConfig.h"

	.global arch_TICK

	.equ INTC_IACKR, ARCH_PPC_E200_INTC_BASE + 0x10
	.equ INTC_EOIR, ARCH_PPC_E200_INTC_BASE + 0x18
	.equ PIT_TFLG3, ARCH_PPC_E200_PIT_BASE + 0x13c
	
	#include "../include/picoRTOS_portasm.S"
	#include "../include/core_e200z7.S"
	
	/* TICK ISR */
	.align 4
	.type arch_TICK, %function
arch_TICK:
	mflr r31            /* save lr in r31 */
	mfsprg r3, 1        /* load task stack pointer from SPRG1 */
	e_bl picoRTOS_tick
	mtsprg 1, r3        /* store returned task stack pointer */
	/* reset timer */
	e_li r7, 1
	e_lis r3, PIT_TFLG3@ha	/* reset timer */
	e_stw r7, PIT_TFLG3@l(r3)
	mtlr r31            /* restore lr */
	se_blr
