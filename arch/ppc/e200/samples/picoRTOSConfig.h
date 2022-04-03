#ifndef PICORTOSCONFIG_H
#define PICORTOSCONFIG_H

/* CLOCKS */
#define CONFIG_SYSCLK_HZ        40000000
#define CONFIG_TICK_HZ          5000

/* TASKS */
#define CONFIG_TASK_COUNT       4

/* STACK */
#define CONFIG_IDLE_STACK_COUNT    ARCH_MIN_STACK_COUNT
#define CONFIG_DEFAULT_STACK_COUNT 256

/* ARCH-specific (MPC574XX) */
#define CONFIG_ARCH_PPC_E200_INTC       0xfc040000
#define CONFIG_ARCH_PPC_E200_INTC_IACKR 0xfc040020
#define CONFIG_ARCH_PPC_E200_INTC_EOIR  0xfc040030
#define CONFIG_ARCH_PPC_E200_TIMER      0xfff84000
#define CONFIG_ARCH_PPC_E200_TIMER_IRQ  229

#endif
