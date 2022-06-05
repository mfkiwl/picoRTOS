	.syntax unified
	
	/* Declarations */
	.global arch_start_first_task
	.global arch_SYSTICK
	.global arch_PENDSV

	.global arch_compare_and_swap
	
	.extern picoRTOS_tick
	.extern picoRTOS_switch_context

	.macro SAVE_CONTEXT rS
	mrs \rS, psp			/* use r0 as psp stack */
	stmdb \rS!, {r4-r11}		/* save registers r4-r11 */
	.endm

	.macro RESTORE_CONTEXT rS
	ldmia \rS!, {r4-r11}	/* restore registers r4-11 */
	msr psp, \rS		/* reclaim stack space */
	.endm
	
	.align 1
	.thumb_func
	.type arch_start_first_task, %function
arch_start_first_task:
	RESTORE_CONTEXT r0
	/* switch to psp stack */
	movs r0, #2
	msr CONTROL, r0
	isb

	pop {r0-r3, r12, lr}	/* pop the registers that are saved automatically */
	pop {r2-r3}		/* return addr and xspr */
	cpsie i			/* enable interrupts */
	bx r2

	.align 1
	.thumb_func
	.type arch_SYSTICK, %function
arch_SYSTICK:
	push {lr}		/* store lr for interrupt return */
	SAVE_CONTEXT r0
	bl picoRTOS_tick
	RESTORE_CONTEXT r0
	pop {pc}	/* return */
	
	.align 1
	.thumb_func
	.type arch_PENDSV, %function
arch_PENDSV:
	push {lr}		/* store lr (interrupt return) */
	SAVE_CONTEXT r0
	bl picoRTOS_switch_context
	RESTORE_CONTEXT r0
	pop {pc}		/* return */

	.align 1
	.thumb_func
	.type arch_compare_and_swap, %function
arch_compare_and_swap:
	dmb ish
	ldrex r3, [r0]
	cmp r3, r1
	bne .arch_compare_and_swap_return
	strex r3, r2, [r0]
	it eq
	moveq r0, r1
	dmb ish
.arch_compare_and_swap_return:
	bx lr
