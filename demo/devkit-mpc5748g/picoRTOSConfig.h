#ifndef PICORTOSCONFIG_H
#define PICORTOSCONFIG_H

/* CLOCKS */
#define CONFIG_SYSCLK_HZ        4000000
#define CONFIG_TICK_HZ          1000

/* TASKS */
#define CONFIG_TASK_COUNT       2
#define TASK_TICK_PRIO          0
#define TASK_LED_PRIO           1

/* STACK */
#define CONFIG_DEFAULT_STACK_COUNT 256

/* ARCH-specific (MPC574XX) */
#define CONFIG_ARCH_PPC_E200_INTC       0xfc040000
#define CONFIG_ARCH_PPC_E200_INTC_IACKR 0xfc040020
#define CONFIG_ARCH_PPC_E200_INTC_EOIR  0xfc040030
#define CONFIG_ARCH_PPC_E200_TIMER      0xfff84000
#define CONFIG_ARCH_PPC_E200_TIMER_IRQ  229

/* DEBUG */
#define CONFIG_CHECK_STACK_INTEGRITY

#endif
