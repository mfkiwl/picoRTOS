#ifndef PICORTOSCONFIG_H
#define PICORTOSCONFIG_H

/* CLOCKS */
#define CONFIG_SYSCLK_HZ        40000000
#define CONFIG_TICK_HZ          1000

/* TASKS */
#define CONFIG_TASK_COUNT       2
#define TASK_TICK_PRIO          0
#define TASK_LED_PRIO           1

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

#endif
