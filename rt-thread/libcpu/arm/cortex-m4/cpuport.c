/*
 * File      : cpuport.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2011, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2011-10-21     Bernard      the first version.
 * 2011-10-27     aozima       update for cortex-M4 FPU.
 * 2011-12-31     aozima       fixed stack align issues.
 * 2012-01-01     aozima       support context switch load/store FPU register.
*/
#include <rtthread.h>

#define USE_FPU   /* ARMCC */ (  (defined ( __CC_ARM ) && defined ( __TARGET_FPU_VFP )) \
                  /* IAR */   || (defined ( __ICCARM__ ) && defined ( __ARMVFP__ )) \
                  /* GNU */   || (defined ( __GNUC__ ) && defined ( __VFP_FP__ ) && !defined(__SOFTFP__)) )

/* exception and interrupt handler table */
rt_uint32_t rt_interrupt_from_thread, rt_interrupt_to_thread;
rt_uint32_t rt_thread_switch_interrupt_flag;

struct exception_stack_frame
{
    rt_uint32_t r0;
    rt_uint32_t r1;
    rt_uint32_t r2;
    rt_uint32_t r3;
    rt_uint32_t r12;
    rt_uint32_t lr;
    rt_uint32_t pc;
    rt_uint32_t psr;

#if USE_FPU
    /* FPU register */
    rt_uint32_t S0;
    rt_uint32_t S1;
    rt_uint32_t S2;
    rt_uint32_t S3;
    rt_uint32_t S4;
    rt_uint32_t S5;
    rt_uint32_t S6;
    rt_uint32_t S7;
    rt_uint32_t S8;
    rt_uint32_t S9;
    rt_uint32_t S10;
    rt_uint32_t S11;
    rt_uint32_t S12;
    rt_uint32_t S13;
    rt_uint32_t S14;
    rt_uint32_t S15;
    rt_uint32_t FPSCR;
    rt_uint32_t NO_NAME;
#endif
};

struct stack_frame
{
    /* r4 ~ r11 register */
    rt_uint32_t r4;
    rt_uint32_t r5;
    rt_uint32_t r6;
    rt_uint32_t r7;
    rt_uint32_t r8;
    rt_uint32_t r9;
    rt_uint32_t r10;
    rt_uint32_t r11;

#if USE_FPU
    /* FPU register s16 ~ s31 */
    rt_uint32_t s16;
    rt_uint32_t s17;
    rt_uint32_t s18;
    rt_uint32_t s19;
    rt_uint32_t s20;
    rt_uint32_t s21;
    rt_uint32_t s22;
    rt_uint32_t s23;
    rt_uint32_t s24;
    rt_uint32_t s25;
    rt_uint32_t s26;
    rt_uint32_t s27;
    rt_uint32_t s28;
    rt_uint32_t s29;
    rt_uint32_t s30;
    rt_uint32_t s31;
#endif

    struct exception_stack_frame exception_stack_frame;
};

rt_uint8_t *rt_hw_stack_init(void *tentry, void *parameter,
                             rt_uint8_t *stack_addr, void *texit)
{
    struct stack_frame * stack_frame;
    rt_uint8_t * stk;
    unsigned long i;

    stk = stack_addr + sizeof(rt_uint32_t);

    stk -= sizeof(struct stack_frame);
    stack_frame = (struct stack_frame *)stk;

    /* init all register */
    for(i=0; i<sizeof(struct stack_frame)/sizeof(rt_uint32_t); i++)
    {
        ((rt_uint32_t*)stack_frame)[i] = 0xdeadbeef;
    }

    stack_frame->exception_stack_frame.r0 = (unsigned long)parameter; /* r0 : argument */
    stack_frame->exception_stack_frame.r1 = 0;                        	/* r1 */
    stack_frame->exception_stack_frame.r2 = 0;                        	/* r2 */
    stack_frame->exception_stack_frame.r3 = 0;                        	/* r3 */
    stack_frame->exception_stack_frame.r12 = 0;                       	/* r12 */
    stack_frame->exception_stack_frame.lr = (unsigned long)texit;     /* lr */
    stack_frame->exception_stack_frame.pc = (unsigned long)tentry;    /* entry point, pc */
    stack_frame->exception_stack_frame.psr = 0x01000000L;             	/* PSR */

    /* return task's current stack address */
    return stk;
}

extern void rt_hw_interrupt_thread_switch(void);
extern long list_thread(void);
extern rt_thread_t rt_current_thread;
void rt_hw_hard_fault_exception(struct exception_stack_frame * exception_stack)
{
    rt_kprintf("psr: 0x%08x\n", exception_stack->psr);
    rt_kprintf(" pc: 0x%08x\n", exception_stack->pc);
    rt_kprintf(" lr: 0x%08x\n", exception_stack->lr);
    rt_kprintf("r12: 0x%08x\n", exception_stack->r12);
    rt_kprintf("r03: 0x%08x\n", exception_stack->r3);
    rt_kprintf("r02: 0x%08x\n", exception_stack->r2);
    rt_kprintf("r01: 0x%08x\n", exception_stack->r1);
    rt_kprintf("r00: 0x%08x\n", exception_stack->r0);

    rt_kprintf("hard fault on thread: %8.*s\n", RT_NAME_MAX, rt_current_thread->name);
#ifdef RT_USING_FINSH
    list_thread();
#endif
    while (1);
}

void rt_hw_cpu_shutdown()
{
    rt_kprintf("shutdown...\n");

    RT_ASSERT(0);
}
