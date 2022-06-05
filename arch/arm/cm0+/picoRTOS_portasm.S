	.syntax unified
	
	/* Declarations */
	.global arch_start_first_task
	.global arch_SYSTICK
	.global arch_PENDSV
	
	.extern picoRTOS_tick
	.extern picoRTOS_switch_context

	.macro SAVE_CONTEXT rS
	mrs \rS, psp
	subs \rS, \rS, #16	/* store low registers */
	stmia \rS!, {r4-r7}
	mov r4, r8
	mov r5, r9
	mov r6, r10
	mov r7, r11
	subs \rS, \rS, #32	/* store high registers */
	stmia \rS!, {r4-r7}
	subs \rS, \rS, #16	/* revert auto-increment */
	.endm

	.macro RESTORE_CONTEXT rS
	ldmia \rS!, {r4-r7} 	/* load high registers */
	mov r8, r4
	mov r9, r5
	mov r10, r6
	mov r11, r7
	ldmia \rS!, {r4-r7}	/* load low registers */
	msr psp, \rS
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

	pop {r0-r5}	/* pop the registers that are saved automatically */
	mov lr, r5	/* lr is copied from r5 */
	pop {r2, r3}	/* return address + xspr */
	cpsie i		/* enable interrupts */
	bx r2		/* finally, jump to the user defined task code */

	.align 1
	.thumb_func
	.type arch_SYSTICK, %function
arch_SYSTICK:
	push {lr}	/* store lr for interrupt return */
	SAVE_CONTEXT r0
	bl picoRTOS_tick
	RESTORE_CONTEXT r0
	pop {pc}	/* return */
	
	.align 1
	.thumb_func
	.type arch_PENDSV, %function
arch_PENDSV:
	push {lr}	/* store lr (interrupt return) */
	SAVE_CONTEXT r0
	bl picoRTOS_switch_context
	RESTORE_CONTEXT r0
	pop {pc}	/* return */
