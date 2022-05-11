	.syntax unified
	
	/* Declarations */
	.global arch_PWM_WRAP

	#include "../picoRTOS_portasm.S"
	
	.equ PWM_INTR, 0x400500a4
	.equ NVIC_ICPR, 0xe000e280

	.align 1
	.thumb_func
	.type arch_PWM_WRAP, %function
arch_PWM_WRAP:
	push {lr}	/* store lr for interrupt return */
	SAVE_CONTEXT

	/* ack pwm */
	ldr r3, =PWM_INTR
	movs r2, #1
	str r2, [r3]
	ldr r3, =NVIC_ICPR
	movs r2, #16
	str r2, [r3]

	mrs r0, psp
	bl picoRTOS_tick
	
	RESTORE_CONTEXT r0
	pop {pc}	/* return */
