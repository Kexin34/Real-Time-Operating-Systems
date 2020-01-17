/**
 * SYSTICK.h
 * ECED 4402
 * SYSTICK ISR layer module
 * Name: Kexin Wen
 * Original Author: Emad Khan, ECED4402 TA
 * Date: 29 Oct 2018
 */
#include "process_management.h"
#include "systick.h"
#include "initialization.h"
#include "kernel_functions.h"
#include "KernelCalls.h"
int clock_sys = 0;
void SysTickStart(void)
{
// Set the clock source to internal and enable the counter to interrupt
ST_CTRL_R |= ST_CTRL_CLK_SRC | ST_CTRL_ENABLE;
}

void SysTickStop(void)
{
// Clear the enable bit to stop the counter
ST_CTRL_R &= ~(ST_CTRL_ENABLE);
}

void SysTickPeriod(unsigned long Period)
{
/*
 For an interrupt, must be between 2 and 16777216 (0x100.0000 or 2^24)
*/
ST_RELOAD_R = Period - 1;  /* 1 to 0xff.ffff */
}

void SysTickIntEnable(void)
{
// Set the interrupt bit in STCTRL
ST_CTRL_R |= ST_CTRL_INTEN;
}

void SysTickIntDisable(void)
{
// Clear the interrupt bit in STCTRL
ST_CTRL_R &= ~(ST_CTRL_INTEN);
}

void IntMasterEnable(void)
{
    // enable CPU interrupts
    __asm(" cpsie   i");
}
int time_count = 0;
/* Trigger PendSV() and increment counter of 'clock_sys' */
void SysTickHandler(void)
{
    clock_sys ++;
    if(clock_sys >=100)
    {
        clock_incr();
        clock_sys = 0;
        if(current_time.sec %4 == 0)
        {
            time_count++;
            char *tttt = "\n\n\r>>TICK<<\n";
            k_send(29,TIME , tttt, strlen(tttt));


        }
    }
    NVIC_INT_CTRL_R |= TRIGGER_PENDSV;

}
void time_notice(void)
{

}

void SysTick_initialization(void)
{
   SysTickPeriod(MAX_WAIT);
   SysTickIntEnable();
   IntMasterEnable();
}

/* Function to increment clock, consider roll up */
void clock_incr(void)
{
    current_time.sec++;
    if(current_time.sec >= 60){
        current_time.sec = 0;
        current_time.min++;
    }
    if(current_time.min >= 60){
        current_time.min = 0;
        current_time.hour++;
    }
    if(current_time.hour == 24){
        current_time.hour = 0;
    }
}

