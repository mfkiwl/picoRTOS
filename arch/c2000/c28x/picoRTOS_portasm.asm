	.if __TI_EABI__
	.asg picoRTOS_tick, _picoRTOS_tick
	.asg picoRTOS_switch_context, _picoRTOS_switch_context
	.endif

	.ref _picoRTOS_tick
	.ref _picoRTOS_switch_context

	.if __TI_EABI__
	.asg arch_INT14, _arch_INT14
	.asg arch_RTOSINT, _arch_RTOSINT
	.asg arch_start_first_task, _arch_start_first_task
	.asg arch_test_and_set, _arch_test_and_set
	.endif

	.global _arch_INT14
	.global _arch_RTOSINT
	.global _arch_start_first_task
	.global _arch_test_and_set
	
SAVE_CONTEXT .macro
	;;  isr relevant registers are auto-saved
	;;  align stack
	asp
	;;  other relevant registers
	push DP:ST1
	push RPC
	push XT
	push AR1H:AR0H
	push XAR2
	push XAR3
	push XAR4
	push XAR5
	push XAR6
	push XAR7
	.endm

RESTORE_CONTEXT .macro
	;;  restore all relevant registers
	pop XAR7
	pop XAR6
	pop XAR5
	pop XAR4
	pop XAR3
	pop XAR2
	pop AR1H:AR0H
	pop XT
	pop RPC
	pop DP:ST1
	;;  un-align stack
	nasp
	;;  iret will unstack the rest
	.endm

_arch_INT14:
	;; tick interrupt
	SAVE_CONTEXT

	mov AR6, @SP
	movz AR4, AR6
	lcr _picoRTOS_tick

	mov @SP, AR4
	RESTORE_CONTEXT
	iret

_arch_RTOSINT:
	;; context switch interrupt
	SAVE_CONTEXT

	mov AR6, @SP
	movz AR4, AR6
	lcr _picoRTOS_switch_context

	mov @SP, AR4
	RESTORE_CONTEXT
	iret

_arch_start_first_task:
	mov @SP, AR4
	RESTORE_CONTEXT
	;; emulate first iret
	subb SP, #1		; align
	pop  RPC
	subb SP, #1		; ignore DBGSTAT
	pop  IER
	subb SP, #2		; ignore DP:ST1
	subb SP, #2		; ignore AR1H:AR0H
	subb SP, #2		; ignore P
	subb SP, #2		; ignore ACC
	pop  T:ST0
	;; don't forget LRETR will unstack RPC
	movl XAR0, #_arch_start_first_task
	push XAR0
	;; enable interrupts
	clrc INTM
	lretr

_arch_test_and_set:
	mov AL, #1
	tset *AR4, #0
	sb arch_test_and_set_return, TC
	mov AL, #0
arch_test_and_set_return:
	lretr
