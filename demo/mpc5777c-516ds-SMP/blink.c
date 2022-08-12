#include "picoRTOS-SMP.h"

#include "ipc/picoRTOS_mutex.h"
#include "ipc/picoRTOS_cond.h"

#include <stdbool.h>
#include "device_registers.h"

#define TICK_R11 210
#define LED_D510 114
#define LED_D511 115

#define LED_DELAY_SHORT PICORTOS_DELAY_MSEC(30)
#define LED_DELAY_LONG  PICORTOS_DELAY_MSEC(60)

#define DEFAULT_DIVC_RATE 12
#define DEFAULT_DIVC_INIT 624
#define DEFAULT_DIVE      16499
#define DEFAULT_DIVS      17535

#define BUSY_NRETRY 1000

static void wait_xosc_provide_stable_clock(void)
{
    int loop = BUSY_NRETRY;
    volatile uint32_t reg;

    do
        reg = SIU->RSR;
    while ((reg & SIU_RSR_XOSC_MASK) == 0 &&
           loop-- != 0);

    /* signal error */
    arch_assert(loop != -1);
}

static void wait_pll0_lock(void)
{
    int loop = BUSY_NRETRY;
    volatile uint32_t reg;

    do
        reg = PLLDIG->PLL0SR;
    while ((reg & PLLDIG_PLL0SR_LOCK_MASK) == 0 &&
           loop-- != 0);

    /* signal error */
    arch_assert(loop != -1);
}

static void wait_pll1_lock(void)
{
    int loop = BUSY_NRETRY;
    volatile uint32_t reg;

    do
        reg = PLLDIG->PLL1SR;
    while ((reg & PLLDIG_PLL1SR_LOCK_MASK) == 0 &&
           loop-- != 0);

    /* signal error */
    arch_assert(loop != -1);
}

static void configure_pll_digital_interfaces(void)
{
    /* turn off plls */
    PLLDIG->PLL0CR &= ~PLLDIG_PLL0CR_CLKCFG_MASK;
    PLLDIG->PLL1CR &= ~PLLDIG_PLL1CR_CLKCFG_MASK;

    /* Configure PLL0 Dividers - 192 MHz from 40 MHz XOSC */
    PLLDIG->PLL0DV = (uint32_t)PLLDIG_PLL0DV_PREDIV(5);
    PLLDIG->PLL0DV |= (uint32_t)PLLDIG_PLL0DV_MFD(48);
    PLLDIG->PLL0DV |= (uint32_t)PLLDIG_PLL0DV_RFDPHI(2);
    PLLDIG->PLL0DV |= (uint32_t)PLLDIG_PLL0DV_RFDPHI1(8);
    /* Turn on PLL0 */
    PLLDIG->PLL0CR |= PLLDIG_PLL0CR_CLKCFG(3);
    wait_pll0_lock();

    /* Configure PLL1 Dividers - 264Mhz from 48Mhz PLL0-PHI1 ref */
    PLLDIG->PLL1DV |= (uint32_t)PLLDIG_PLL1DV_MFD(22);
    PLLDIG->PLL1DV |= (uint32_t)PLLDIG_PLL1DV_RFDPHI(2);
    /* Turn on PLL1 */
    PLLDIG->PLL1CR |= PLLDIG_PLL1CR_CLKCFG(3);
    wait_pll1_lock();
}

static void wait_until_pcs_finishes(void)
{
    int loop = BUSY_NRETRY;
    volatile uint32_t reg;

    do
        reg = SIU->PCSIFR;
    while ((reg & SIU_PCSIFR_PCSI_MASK) == 0 &&
           (reg & SIU_PCSIFR_PCSMS_MASK) != SIU_PCSIFR_PCSMS(3) &&
           loop-- != 0);

    /* signal error */
    arch_assert(loop != -1);

    /* ack interrupt */
    SIU->PCSIFR |= SIU_PCSIFR_PCSI_MASK;
}

static void clock_init(void)
{
    /* run sys clk from IRC */
    SIU->SYSDIV = (uint32_t) ~SIU_SYSDIV_SYSCLKSEL_MASK;

    /* prepare clocks for 264Mhz */

    PCS_0->SDUR = (uint32_t)16; /* 16 IRC clock cycles switch step duration */

    /* div #1 */
    PCS_0->DIVC1 = (uint32_t)PCS_0_DIVC1_RATE(DEFAULT_DIVC_RATE);   /* div change rate XOSC */
    PCS_0->DIVC1 |= PCS_0_DIVC1_INIT(DEFAULT_DIVC_INIT);            /* div change innit value XOSC */
    PCS_0->DIVE1 = (uint32_t)PCS_0_DIVE1_DIVE(DEFAULT_DIVE);        /* div end value for XOSC */
    PCS_0->DIVS1 = (uint32_t)PCS_0_DIVS1_DIVS(DEFAULT_DIVS);        /* div start value for XOSC */
    /* div #2 */
    PCS_0->DIVC2 = (uint32_t)PCS_0_DIVC2_RATE(DEFAULT_DIVC_RATE);   /* div change rate XOSC */
    PCS_0->DIVC2 |= PCS_0_DIVC2_INIT(DEFAULT_DIVC_INIT);            /* div change innit value XOSC */
    PCS_0->DIVE2 = (uint32_t)PCS_0_DIVE2_DIVE(DEFAULT_DIVE);        /* div end value for XOSC */
    PCS_0->DIVS2 = (uint32_t)PCS_0_DIVS2_DIVS(DEFAULT_DIVS);        /* div start value for XOSC */
    /* div #3 */
    PCS_0->DIVC3 = (uint32_t)PCS_0_DIVC3_RATE(DEFAULT_DIVC_RATE);   /* div change rate XOSC */
    PCS_0->DIVC3 |= PCS_0_DIVC3_INIT(DEFAULT_DIVC_INIT);            /* div change innit value XOSC */
    PCS_0->DIVE3 = (uint32_t)PCS_0_DIVE3_DIVE(DEFAULT_DIVE);        /* div end value for XOSC */
    PCS_0->DIVS3 = (uint32_t)PCS_0_DIVS3_DIVS(DEFAULT_DIVS);        /* div start value for XOSC */

    /* wait for xosc */
    wait_xosc_provide_stable_clock();

    /* Setup plls */
    configure_pll_digital_interfaces();

    /* Select clock dividers and sources */

    /* XOSC <-> PLL0 in */
    SIU->SYSDIV &= SIU_SYSDIV_PLL0SEL_MASK;
    /* PLL0-PHI1 <-> PLL1 int */
    SIU->SYSDIV &= ~SIU_SYSDIV_PLL1SEL_MASK;
    SIU->SYSDIV |= SIU_SYSDIV_PLL1SEL(1);
    /* PERCLK = PLL0 */
    SIU->SYSDIV &= ~SIU_SYSDIV_PERCLKSEL_MASK;
    SIU->SYSDIV |= SIU_SYSDIV_PERCLKSEL(1);
    /* FMPERDIV is 0 */
    SIU->SYSDIV &= ~SIU_SYSDIV_FMPERDIV_MASK;
    /* PERDIV = 0 */
    SIU->SYSDIV &= ~SIU_SYSDIV_PERDIV_MASK;
    /* MCANSEL <-> XOSC */
    SIU->SYSDIV &= ~SIU_SYSDIV_MCANSEL_MASK;
    /* SYSCLKSEL <-> PLL1 */
    SIU->SYSDIV &= ~SIU_SYSDIV_SYSCLKSEL_MASK;
    SIU->SYSDIV |= SIU_SYSDIV_SYSCLKSEL(2);
    /* ETPUDIV = 1 */
    SIU->SYSDIV &= ~SIU_SYSDIV_ETPUDIV_MASK;
    SIU->SYSDIV |= SIU_SYSDIV_ETPUDIV(1);
    /* SYSCLKDIV = 4 (value 1) */
    SIU->SYSDIV &= ~SIU_SYSDIV_SYSCLKDIV_MASK;
    SIU->SYSDIV |= SIU_SYSDIV_SYSCLKDIV(1);

    /* Enable PCS */
    SIU->SYSDIV |= SIU_SYSDIV_PCSEN(1);
    wait_until_pcs_finishes();

    /* ECCR */
    SIU->ECCR = (uint32_t)SIU_ECCR_ENGDIV(16); /* div by 32 */
    SIU->ECCR |= SIU_ECCR_ECCS(0);
    SIU->ECCR |= SIU_ECCR_EBDF(2);
    /* SDCLK */
    SIU->SDCLKCFG = (uint32_t)SIU_SDCLKCFG_SDDIV(14); /* div by 15 */
    /* LFCLK */
    SIU->LFCLKCFG = (uint32_t)SIU_LFCLKCFG_LFCLKSEL(0);
    SIU->LFCLKCFG |= (uint32_t)SIU_LFCLKCFG_LFDIV(1);   /* div by 2 */
    /* PSCLK */
    SIU->PSCLKCFG = (uint32_t)SIU_PSCLKCFG_PSDIV(2);    /* div by 3 */
    SIU->PSCLKCFG |= SIU_PSCLKCFG_PSDIV1M(239);         /* div by 240 */
}

static void set_gpio_output(unsigned long pin)
{
    /* pa */
    SIU->PCR[pin] &= ~SIU_PCR_PA_MASK;
    /* obe */
    SIU->PCR[pin] |= SIU_PCR_OBE((uint32_t)1);
    SIU->PCR[pin] &= ~SIU_PCR_IBE_MASK;
    /* dsc */
    SIU->PCR[pin] |= SIU_PCR_DSC_MASK;
    /* src */
    SIU->PCR[pin] |= SIU_PCR_SRC_MASK;
}

static void hw_init(void)
{
    clock_init();

    /* LEDs */
    set_gpio_output(LED_D510);
    set_gpio_output(LED_D511);

    /* TICK */
    set_gpio_output(TICK_R11);
}

static void set_gpio(unsigned long pin, bool status)
{
    SIU->GPDO[pin] = (uint8_t)status;
}

static void set_led(unsigned long led, bool status, picoRTOS_tick_t delay)
{
    set_gpio(led, status);
    picoRTOS_sleep(delay);
}

static void deepcall_schedule(unsigned long n)
{
    if (n != 0)
        deepcall_schedule(n - 1);
    else
        picoRTOS_schedule();
}

static void tick_main(void *priv)
{
    arch_assert(priv == NULL);

    bool x = false;

    for (;;) {
        set_gpio(TICK_R11, x);

        x = !x;
        picoRTOS_schedule();
    }
}

/* IPC test */
static struct picoRTOS_mutex mutex = PICORTOS_MUTEX_INITIALIZER;
static struct picoRTOS_cond cond = PICORTOS_COND_INITIALIZER;

static void led0_main(void *priv)
{
    arch_assert(priv == NULL);

    picoRTOS_tick_t ref = picoRTOS_get_tick();

    for (;;) {
        picoRTOS_sleep_until(&ref, PICORTOS_DELAY_SEC(1));

        picoRTOS_mutex_lock(&mutex);

        /* turn on */
        set_led(LED_D510, true, LED_DELAY_SHORT);
        /* turn off */
        set_led(LED_D510, false, LED_DELAY_SHORT);
        /* on again */
        set_led(LED_D510, true, LED_DELAY_LONG);
        /* off */
        set_led(LED_D510, false, 0);

        /* ipc */
        picoRTOS_cond_signal(&cond);
        picoRTOS_mutex_unlock(&mutex);

        /* stack test */
        deepcall_schedule(10);
    }
}

static void led1_main(void *priv)
{
    arch_assert(priv == NULL);

    for (;;) {
        picoRTOS_mutex_lock(&mutex);
        picoRTOS_cond_wait(&cond, &mutex);

        /* turn on */
        set_led(LED_D511, true, LED_DELAY_SHORT);
        /* turn off */
        set_led(LED_D511, false, LED_DELAY_SHORT);
        /* on again */
        set_led(LED_D511, true, LED_DELAY_LONG);
        /* off */
        set_led(LED_D511, false, 0);

        picoRTOS_mutex_unlock(&mutex);

        /* stack test */
        deepcall_schedule(20);
    }
}

int main(void)
{
    hw_init();
    picoRTOS_init();

    struct picoRTOS_task task;
    static picoRTOS_stack_t stack0[CONFIG_DEFAULT_STACK_COUNT];
    static picoRTOS_stack_t stack1[CONFIG_DEFAULT_STACK_COUNT];
    static picoRTOS_stack_t stack2[CONFIG_DEFAULT_STACK_COUNT];

    picoRTOS_task_init(&task, tick_main, NULL, stack0, CONFIG_DEFAULT_STACK_COUNT);
    picoRTOS_add_task(&task, TASK_TICK_PRIO);

    picoRTOS_task_init(&task, led0_main, NULL, stack1, CONFIG_DEFAULT_STACK_COUNT);
    picoRTOS_add_task(&task, TASK_LED0_PRIO);
    picoRTOS_SMP_set_core_mask(TASK_LED0_PRIO, 0x1);

    picoRTOS_task_init(&task, led1_main, NULL, stack2, CONFIG_DEFAULT_STACK_COUNT);
    picoRTOS_add_task(&task, TASK_LED1_PRIO);
    picoRTOS_SMP_set_core_mask(TASK_LED1_PRIO, 0x2);

    picoRTOS_start();

    /* not supposed to end there */
    arch_assert(false);
    return 1;
}
