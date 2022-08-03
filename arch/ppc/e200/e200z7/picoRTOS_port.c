#include "picoRTOS.h"

/* preprocessor check */
#ifndef ARCH_PPC_E200_INTC_BASE
# error "INTC_BASE is not defined"
#endif
#ifndef ARCH_PPC_E200_PIT_BASE
# error "PIT_BASE is not defined"
#endif
#ifndef ARCH_PPC_E200_PIT_IRQ
# error "PIT_IRQ is not defined"
#endif

#define INTC_BASE   ARCH_PPC_E200_INTC_BASE
#define PIT_BASE    ARCH_PPC_E200_PIT_BASE
#define PIT_IRQ     ARCH_PPC_E200_PIT_IRQ

#define INTC_MCR   ((volatile unsigned long*)INTC_BASE)
#define INTC_CPR   ((volatile unsigned long*)(INTC_BASE + 0x8))
#define INTC_IACKR ((volatile unsigned long*)(INTC_BASE + 0x10))
#define INTC_EOIR  ((volatile unsigned long*)(INTC_BASE + 0x18))
#define INTC_PSR   ((volatile unsigned char*)(INTC_BASE + 0x40))

#define PIT_MCR     ((volatile unsigned long*)PIT_BASE)
/* channel 3 */
#define PIT_LDVAL3  ((volatile unsigned long*)(PIT_BASE + 0x130))
#define PIT_CVAL3   ((volatile unsigned long*)(PIT_BASE + 0x134))
#define PIT_TCTRL3  ((volatile unsigned long*)(PIT_BASE + 0x138))
#define PIT_TFLG3   ((volatile unsigned long*)(PIT_BASE + 0x13c))

/* MSR */
#define ARCH_MSR_SPE (1ul << 25)
#define ARCH_MSR_CE  (1ul << 17)
#define ARCH_MSR_EE  (1ul << 15)
#define ARCH_MSR_ME  (1ul << 12)

#define ARCH_MSR_DEFAULT \
    (ARCH_MSR_SPE | ARCH_MSR_CE | ARCH_MSR_EE | ARCH_MSR_ME)

/* ASM */
/*@external@*/ extern void arch_TICK(void);
/*@external@*/ extern unsigned long arch_MSR(void);
/*@external@*/ extern void arch_start_first_task(picoRTOS_stack_t *sp);
/*@external@*/ extern picoRTOS_atomic_t arch_compare_and_swap(picoRTOS_atomic_t *var,
                                                              picoRTOS_atomic_t old,
                                                              picoRTOS_atomic_t val);
/* FUNCTIONS TO IMPLEMENT */

static void timer_init(void)
{
    *PIT_MCR &= ~0x2;       /* enable PIT */
    *PIT_LDVAL3 = (unsigned long)(CONFIG_SYSCLK_HZ / CONFIG_TICK_HZ) - 1ul;
    *PIT_TCTRL3 |= 0x3;     /* enable interrupt & start */
}

static void intc_init(void)
{
    unsigned long *VTBA = (unsigned long*)(*INTC_IACKR & 0xfffff000);

    *INTC_MCR = 0;
    *INTC_CPR = 0;

    /* TICK */
    VTBA[PIT_IRQ] = (unsigned long)arch_TICK;
    /* priority 1 on any core */
    INTC_PSR[PIT_IRQ] = (unsigned char)0xc1u;
}

void arch_init(void)
{
    /* disable interrupts */
    ASM("wrteei 0");

    /* INTERRUPTS are statically managed in picoRTOS_common.S */

    /* TIMER */
    timer_init();

    /* INTERRUPT CONTROLLER */
    intc_init();
}

void arch_suspend(void)
{
    /* disable tick */
    ASM("wrteei 0");
}

void arch_resume(void)
{
    /* enable tick */
    ASM("wrteei 1");
}

picoRTOS_stack_t *arch_prepare_stack(struct picoRTOS_task *task)
{
    /* decrementing stack */
    picoRTOS_stack_t *sp = task->stack + task->stack_count;

    /* Allocate stack */
    sp -= ARCH_INITIAL_STACK_COUNT;

    /* r[14-31] : 18 registers */
    /* r[0, 3-12] : 11 registers */
    sp[7] = (picoRTOS_stack_t)task->priv;       /* r3 */
    /* xer, xtr, lr, cr : 4 registers */
    sp[5] = 0;                                  /* xer */
    sp[4] = 0;                                  /* ctr */
    sp[3] = 0;                                  /* lr */
    sp[2] = 0;                                  /* cr */
    /* srr[0-1] : 2 registers */
    sp[1] = arch_MSR() | ARCH_MSR_DEFAULT;      /* srr1 */
    sp[0] = (picoRTOS_stack_t)task->fn;         /* srr0 / pc */

    return sp;
}

void arch_yield(void)
{
    ASM("se_sc");
}

void arch_idle(void *null)
{
    arch_assert(null == NULL);

    for (;;)
        ASM("wait");
}

/* ATOMIC */

picoRTOS_atomic_t arch_test_and_set(picoRTOS_atomic_t *ptr)
{
    return arch_compare_and_swap(ptr, 0, (picoRTOS_atomic_t)1);
}
