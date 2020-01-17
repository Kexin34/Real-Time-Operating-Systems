/*
 * process_management.c
 * Class: ECED 4402
 * Assignment#2
 * Author Name: Kexin Wen
 * Date: 06 Nov 2018
 * Purpose: Provides function definitions for registering,
 *          and swapping processes including the saving and
 *          restoring of process state.
*/
#include <stdio.h>
#include "process_management.h"
#include "KernelCalls.h"

/* Register a new instance of a process with a specified priority and unique PID */
void reg_proc(void(*func_name)(), unsigned pid, unsigned xpriority)
{
    /* Allocating stack space, with initial register values */
    unsigned long stktop = (unsigned long)malloc(STACKSIZE);
    /* create a new stack frame for this process and initialize its registers */
    struct stack_frame *stktop2;
    stktop2 = (struct stack_frame *)stktop + STACKSIZE - sizeof(struct stack_frame);
    stktop2->r0 = 0x0;
    stktop2->pc = (unsigned long)func_name;     /* addr of process entry point */
    stktop2->psr = THUMB_MODE;                   /* Indicating THUMB mode */
    stktop2->lr = (unsigned long)&terminate;

    /* Create process control block */
    struct pcb *pcbptr;
    pcbptr = (struct pcb *)malloc(sizeof(struct pcb));
    pcbptr->priority = xpriority;
    pcbptr->id = pid;
    pcbptr->sp = (unsigned long)stktop2;

    if(xpriority > priority)
        priority = xpriority;
    /* If process queue is empty, link its next/prev to ifself */
    if (running[xpriority] == NULL)
    {
        running[xpriority] = pcbptr;
        running[xpriority]-> next = running[xpriority];
        running[xpriority]-> prev = running[xpriority];
    }
    /* Link to adjacent process in the waiting-to-run queue */
    else
    {
        pcbptr->next = running[xpriority]->next;
        running[xpriority]-> next -> prev = pcbptr;
        running[xpriority]->next = pcbptr;
        pcbptr-> prev = running[xpriority];
        running[xpriority] = pcbptr;
    }
}

unsigned long get_PSP(void)
{
/* Returns contents of PSP (current process stack */
__asm(" mrs     r0, psp");
__asm(" bx  lr");
return 0;   /***** Not executed -- shuts compiler up */
        /***** If used, will clobber 'r0' */
}

unsigned long get_MSP(void)
{
/* Returns contents of MSP (main stack) */
__asm(" mrs     r0, msp");
__asm(" bx  lr");
return 0;
}

void set_PSP(volatile unsigned long ProcessStack)
{
/* set PSP to ProcessStack */
__asm(" msr psp, r0");
}

void set_MSP(volatile unsigned long MainStack)
{
/* Set MSP to MainStack */
__asm(" msr msp, r0");
}

void volatile save_registers()
{
/* Save r4..r11 on process stack */
__asm(" mrs     r0,psp");
/* Store multiple, decrement before; '!' - update R0 after each store */
__asm(" stmdb   r0!,{r4-r11}");
__asm(" msr psp,r0");
}

void volatile restore_registers()
{
/* Restore r4..r11 from stack to CPU */
__asm(" mrs r0,psp");
/* Load multiple, increment after; '!' - update R0 */
__asm(" ldmia   r0!,{r4-r11}");
__asm(" msr psp,r0");
}

unsigned long get_SP()
{
/**** Leading space required -- for label ****/
__asm("     mov     r0,SP");
__asm(" bx  lr");
return 0;
}



