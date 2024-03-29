#ifndef PICORTOSCONFIG_H
#define PICORTOSCONFIG_H

/* CLOCKS */
#define CONFIG_SYSCLK_HZ        4000000
#define CONFIG_TICK_HZ          1000

/* TASKS */
#define CONFIG_TASK_COUNT       3
#define TASK_TICK_PRIO          0
#define TASK_LED0_PRIO          1
#define TASK_LED1_PRIO          2

/* STACK */
#define CONFIG_DEFAULT_STACK_COUNT 256

/* SMP */
#define CONFIG_SMP_CORES          3
#define CONFIG_DEADLOCK_COUNT     1000

/* DEBUG */
#define CONFIG_CHECK_STACK_INTEGRITY

/* PPC specific */
#define ARCH_PPC_E200_INTC_BASE   0xfc040000
#define ARCH_PPC_E200_PIT_BASE    0xfff84000
#define ARCH_PPC_E200_PIT_IRQ     229

/* PPC-SMP specific */
#define ARCH_PPC_E200_SEMA4_BASE 0xfc03c000
#define ARCH_PPC_E200_MC_ME_BASE 0xfffb8000
#define ARCH_PPC_E200_SIUL_BASE  0xfffc0000

#endif
