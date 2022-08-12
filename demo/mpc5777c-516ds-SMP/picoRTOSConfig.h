#ifndef PICORTOSCONFIG_H
#define PICORTOSCONFIG_H

/* SMP */
#define CONFIG_SMP_CORES 2

/* CLOCKS */
#define CONFIG_SYSCLK_HZ        40000000
#define CONFIG_TICK_HZ          1000

/* TASKS */
#define CONFIG_TASK_COUNT       3
#define TASK_TICK_PRIO          0
#define TASK_LED0_PRIO          1
#define TASK_LED1_PRIO          2

/* STACK */
#define CONFIG_DEFAULT_STACK_COUNT 256

/* MUTEX */
#define CONFIG_DEADLOCK_COUNT 1000

/* DEBUG */
#define CONFIG_CHECK_STACK_INTEGRITY

/* PPC specific */
#define ARCH_PPC_E200_INTC_BASE   0xfff48000
#define ARCH_PPC_E200_PIT_BASE    0xc3ff0000
#define ARCH_PPC_E200_PIT_IRQ     304

/* PPC-SMP specific */
#define ARCH_PPC_E200_SEMA4_BASE 0xfff24000
#define ARCH_PPC_E200_SIU_BASE   0xc3f90000

#endif
