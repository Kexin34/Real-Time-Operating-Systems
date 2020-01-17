/*
 * process_management.h
 * Class: ECED 4402
 * Assignment#2
 * Author Name: Kexin Wen
 * Date: 06 Nov 2018
 * Purpose: Contain the structure of stack frame and PCB
 *          Set process management related function
*/
#ifndef PROCESS_H_
#define PROCESS_H_

#define TRUE    1
#define FALSE   0
#define PRIVATE static

#define SVC()   __asm(" SVC #0")
#define disable()   __asm(" cpsid i")
#define enable()    __asm(" cpsie i")

#define MSP_RETURN 0xFFFFFFF9    //LR value: exception return using MSP as SP
#define PSP_RETURN 0xFFFFFFFD    //LR value: exception return using PSP as SP

#define THUMB_MODE 0x01000000
#define MAX_PRIORITY    6
#define STACKSIZE   1024
#define PCBSIZE     200

/* Cortex default stack frame */
struct stack_frame
{
/* Registers saved by s/w (explicit) */
/* There is no actual need to reserve space for R4-R11, other than
 * for initialization purposes.  Note that r0 is the h/w top-of-stack.
 */
unsigned long r4;
unsigned long r5;
unsigned long r6;
unsigned long r7;
unsigned long r8;
unsigned long r9;
unsigned long r10;
unsigned long r11;
/* Stacked by hardware (implicit)*/
unsigned long r0;
unsigned long r1;
unsigned long r2;
unsigned long r3;
unsigned long r12;
unsigned long lr;
unsigned long pc;
unsigned long psr;
};

/* Process control block */
struct pcb
{
    /* Stack pointer - r13 (PSP) */
    unsigned long sp;
    int id;
    int priority;
    /* Links to adjacent PCBs */
    struct pcb *next;
    struct pcb *prev;
    int state;  /* Process states:  BLOCKED, RUNNING, WAIT */
};

void set_LR(volatile unsigned long);
unsigned long get_PSP();
void set_PSP(volatile unsigned long);
unsigned long get_MSP(void);
void set_MSP(volatile unsigned long);
unsigned long get_SP();
void volatile save_registers();
void volatile restore_registers();

extern struct pcb *running[MAX_PRIORITY];
extern long priority;
extern void reg_proc(void(*func_name)(), unsigned pid, unsigned xpriority);

#endif /*PROCESS_H_*/
