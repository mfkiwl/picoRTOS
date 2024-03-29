#include "picoRTOS.h"
#include "picoRTOS_port.h"

/* CHECK FOR OBVIOUS ERRORS */

#if CONFIG_DEFAULT_STACK_COUNT < ARCH_MIN_STACK_COUNT
# error Default stack is too small
#endif

/* TASKS */

void picoRTOS_task_init(struct picoRTOS_task *task,
                        picoRTOS_task_fn_t fn, void *priv,
                        picoRTOS_stack_t *stack,
                        size_t stack_count)
{
    arch_assert(stack_count >= (size_t)ARCH_MIN_STACK_COUNT);

    task->fn = fn;
    task->stack = stack;
    task->stack_count = stack_count;
    task->priv = priv;
}

/* SCHEDULER main structures */

typedef enum {
    PICORTOS_TASK_STATE_EMPTY,
    PICORTOS_TASK_STATE_READY,
    PICORTOS_TASK_STATE_SLEEP
} picoRTOS_task_state_t;

struct picoRTOS_task_core {
    /*@temp@*/ picoRTOS_stack_t *sp;
    picoRTOS_task_state_t state;
    picoRTOS_tick_t tick;
#ifdef CONFIG_CHECK_STACK_INTEGRITY
    /*@temp@*/ picoRTOS_stack_t *stack;
    size_t stack_count;
#endif
};

/* user-defined tasks + idle */
#define TASK_COUNT      (CONFIG_TASK_COUNT + 1)
#define TASK_IDLE_PRIO  (TASK_COUNT - 1)

struct picoRTOS_core {
    int is_running;
    size_t index;
    volatile picoRTOS_tick_t tick;
    struct picoRTOS_task_core task[TASK_COUNT];
    /* IDLE */
    picoRTOS_stack_t idle_stack[ARCH_MIN_STACK_COUNT];
};

/* main core component */
static struct picoRTOS_core picoRTOS;

/* SCHEDULER functions */

void picoRTOS_init(void)
{
    /* zero all tasks with no memset */
    size_t n = (size_t)TASK_COUNT;

    while (n-- != 0) {
        picoRTOS.task[n].sp = NULL;
        picoRTOS.task[n].state = PICORTOS_TASK_STATE_EMPTY;
        picoRTOS.task[n].tick = 0;
#ifdef CONFIG_CHECK_STACK_INTEGRITY
        picoRTOS.task[n].stack = NULL;
        picoRTOS.task[n].stack_count = 0;
#endif
    }

    /* IDLE */
    struct picoRTOS_task idle;

    picoRTOS_task_init(&idle, arch_idle, NULL, picoRTOS.idle_stack,
                       (size_t)ARCH_MIN_STACK_COUNT);

    picoRTOS_add_task(&idle, (picoRTOS_priority_t)TASK_IDLE_PRIO);
    picoRTOS.index = (size_t)TASK_IDLE_PRIO; /* starts at idle */
    picoRTOS.tick = 0;

    /* RTOS status */
    picoRTOS.is_running = 0;
}

void picoRTOS_add_task(struct picoRTOS_task *task, picoRTOS_priority_t prio)
{
    /* check params */
    arch_assert(prio < (picoRTOS_priority_t)TASK_COUNT);
    arch_assert(picoRTOS.task[prio].state == PICORTOS_TASK_STATE_EMPTY);

    picoRTOS.task[prio].sp = arch_prepare_stack(task);
    picoRTOS.task[prio].state = PICORTOS_TASK_STATE_READY;
#ifdef CONFIG_CHECK_STACK_INTEGRITY
    picoRTOS.task[prio].stack = task->stack;
    picoRTOS.task[prio].stack_count = task->stack_count;
#endif
}

void picoRTOS_start(void)
{
    arch_init();
    picoRTOS.is_running = 1;
    arch_start_first_task(picoRTOS.task[TASK_IDLE_PRIO].sp);
}

void picoRTOS_suspend()
{
    arch_suspend();
}

void picoRTOS_resume()
{
    arch_resume();
}

void picoRTOS_schedule(void)
{
    arch_yield();
}

void picoRTOS_sleep(picoRTOS_tick_t delay)
{
    struct picoRTOS_task_core *task = &picoRTOS.task[picoRTOS.index];

    arch_assert(picoRTOS.is_running != 0);

    if (delay > 0) {
        arch_suspend();
        task->tick = picoRTOS.tick + delay;
        task->state = PICORTOS_TASK_STATE_SLEEP;
        arch_resume();
    }

    arch_yield();
}

void picoRTOS_sleep_until(picoRTOS_tick_t *ref, picoRTOS_tick_t period)
{
    struct picoRTOS_task_core *task = &picoRTOS.task[picoRTOS.index];

    arch_assert(period > 0);
    arch_assert(picoRTOS.is_running != 0);

    arch_suspend();

    /* check the clock */
    if ((picoRTOS.tick - *ref) < period) {
        *ref = *ref + period;
        task->tick = *ref;
        task->state = PICORTOS_TASK_STATE_SLEEP;
    }else
        /* missed the clock: reset to tick */
        *ref = picoRTOS.tick;

    arch_resume();
    arch_yield();
}

void picoRTOS_kill(void)
{
    arch_suspend();
    picoRTOS.task[picoRTOS.index].state = PICORTOS_TASK_STATE_EMPTY;
    arch_resume();

    arch_yield();
}

int picoRTOS_join(picoRTOS_priority_t prio, picoRTOS_tick_t delay)
{
    arch_assert(prio < (picoRTOS_priority_t)CONFIG_TASK_COUNT);
    arch_assert(delay > 0);

    while (picoRTOS.task[prio].state != PICORTOS_TASK_STATE_EMPTY &&
           --delay != 0 )
        arch_yield();

    if (delay == 0)
        return -1;

    return 0;
}

picoRTOS_priority_t picoRTOS_self(void)
{
    return (picoRTOS_priority_t)picoRTOS.index;
}

picoRTOS_stack_t *picoRTOS_switch_context(picoRTOS_stack_t *sp)
{
    arch_assert(picoRTOS.index < (size_t)CONFIG_TASK_COUNT);

#ifdef CONFIG_CHECK_STACK_INTEGRITY
    arch_assert(sp != NULL);
    arch_assert(sp >= picoRTOS.task[picoRTOS.index].stack);
    arch_assert(sp < (picoRTOS.task[picoRTOS.index].stack +
                      picoRTOS.task[picoRTOS.index].stack_count));
#endif

    /* store current sp */
    picoRTOS.task[picoRTOS.index].sp = sp;

    /* choose next task to run */
    do
        picoRTOS.index++;
    /* ignore sleeping and empty tasks */
    while (picoRTOS.index < (size_t)TASK_IDLE_PRIO &&
           picoRTOS.task[picoRTOS.index].state != PICORTOS_TASK_STATE_READY);

    return picoRTOS.task[picoRTOS.index].sp;
}

picoRTOS_stack_t *picoRTOS_tick(picoRTOS_stack_t *sp)
{
#ifdef CONFIG_CHECK_STACK_INTEGRITY
    arch_assert(sp != NULL);
    arch_assert(sp >= picoRTOS.task[picoRTOS.index].stack);
    arch_assert(sp < (picoRTOS.task[picoRTOS.index].stack +
                      picoRTOS.task[picoRTOS.index].stack_count));
#endif

    /* store current sp */
    picoRTOS.task[picoRTOS.index].sp = sp;

    /* advance tick */
    picoRTOS.tick++;

    /* quick pass on sleeping tasks + idle */
    size_t n = (size_t)TASK_COUNT;

    while (n-- != 0) {
        if (picoRTOS.task[n].state == PICORTOS_TASK_STATE_SLEEP &&
            picoRTOS.task[n].tick == picoRTOS.tick)
            /* task is ready to rumble */
            picoRTOS.task[n].state = PICORTOS_TASK_STATE_READY;

        /* select highest priority ready task */
        if (picoRTOS.task[n].state == PICORTOS_TASK_STATE_READY)
            picoRTOS.index = n;
    }

    return picoRTOS.task[picoRTOS.index].sp;
}

picoRTOS_tick_t picoRTOS_get_tick(void)
{
    return picoRTOS.tick;
}
