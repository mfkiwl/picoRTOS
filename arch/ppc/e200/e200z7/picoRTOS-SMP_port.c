#include "picoRTOS-SMP.h"
#include "picoRTOS-SMP_port.h"

#ifndef CONFIG_DEADLOCK_COUNT
# error Deadlock count is not defined
#endif

#ifndef ARCH_PPC_E200_INTC_BASE
# error "INTC_BASE is not defined"
#endif
#ifndef ARCH_PPC_E200_SEMA4_BASE
# error "SEMA4_BASE is not defined"
#endif
#ifndef ARCH_PPC_E200_SIU_BASE
# error "SIU_BASE is not defined"
#endif
#ifndef ARCH_PPC_E200_PIT_IRQ
# error "PIT_IRQ is not defined"
#endif

/* regs */
#define INTC_BASE   ARCH_PPC_E200_INTC_BASE
#define SEMA42_BASE ARCH_PPC_E200_SEMA4_BASE
#define SIU_BASE    ARCH_PPC_E200_SIU_BASE

#define INTC_CPR   ((volatile unsigned long*)(INTC_BASE + 0x8))
#define INTC_IACKR ((volatile unsigned long*)(INTC_BASE + 0x10))
#define INTC_PSR   ((volatile unsigned char*)(INTC_BASE + 0x40))

#define SEMA42_GATE0 ((unsigned char*)SEMA42_BASE)
#define SEMA42_RSTGT ((unsigned short*)(SEMA42_BASE + 0x100))

#define SIU_HLT1    ((unsigned long*)(SIU_BASE + 0x9a4))
#define SIU_RSTVEC1 ((unsigned long*)(SIU_BASE + 0x9b0))

/* ASM */
/*@external@*/ /*@temp@*/ extern unsigned long *arch_IVPR(void);
/*@external@*/ extern unsigned long arch_R13(void);
/*@external@*/ extern picoRTOS_core_t arch_core(void);
/*@external@*/ extern void arch_memory_barrier(void);
/*@external@*/ extern void arch_core_start(void);

/*@external@*/ extern picoRTOS_stack_t *arch_core_sp;
/*@external@*/ extern picoRTOS_stack_t *arch_task_sp;
/*@external@*/ extern unsigned long *arch_core_ivpr;
/*@external@*/ extern unsigned long arch_core_r13;

static void smp_intc_init(void)
{
    size_t n = (size_t)CONFIG_SMP_CORES;
    unsigned long *VTBA = (unsigned long*)(*INTC_IACKR & 0xfffff000);

    while (n-- != 0) {
        INTC_CPR[n] = 0;
        INTC_IACKR[n] = (unsigned long)VTBA;
    }

    /* priority 1 on any core */
    INTC_PSR[ARCH_PPC_E200_PIT_IRQ] = (unsigned char)0x41u;
}

void arch_smp_init(void)
{
    /* single-core */
    arch_init();

    /* reset spinlock gate0 */
    *SEMA42_RSTGT = (unsigned short)0xe200;
    *SEMA42_RSTGT = (unsigned short)0x1d00;

    /* init intc */
    smp_intc_init();
}

void arch_core_init(picoRTOS_core_t core,
                    picoRTOS_stack_t *stack,
                    size_t stack_count,
                    picoRTOS_stack_t *sp)
{
    arch_assert(core > 0);
    arch_assert(core < (picoRTOS_core_t)CONFIG_SMP_CORES);
    arch_assert(stack != NULL);
    arch_assert(stack_count >= (size_t)ARCH_MIN_STACK_COUNT);
    arch_assert(sp != NULL);

    arch_core_sp = stack + (stack_count - 1);
    arch_task_sp = sp;
    arch_core_ivpr = arch_IVPR();
    arch_core_r13 = arch_R13();

    /* start, rst + vle */
    *SIU_RSTVEC1 = (unsigned long)arch_core_start | 0x1;
}

void arch_spin_lock(void)
{
    int n = CONFIG_DEADLOCK_COUNT;
    picoRTOS_core_t core = arch_core() + 1;

    do
        *SEMA42_GATE0 = (unsigned char)core;
    while (*SEMA42_GATE0 != (unsigned char)core &&
           n-- != 0);

    /* potential deadlock */
    arch_assert(n != -1);
}

void arch_spin_unlock(void)
{
    *SEMA42_GATE0 = (unsigned char)0;
}
