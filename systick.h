/**
 * SYSTICK.h
 * ECED 4402
 * SYSTICK ISR layer module header file
 * Name: Kexin Wen
 * Original Author: Emad Khan, ECED4402 TA
 * Date: 02 Oct 2018
 */

#define ST_CTRL_R   (*((volatile unsigned long *)0xE000E010))
// Systick Reload Value Register (STRELOAD)
#define ST_RELOAD_R (*((volatile unsigned long *)0xE000E014))

// SysTick defines
#define ST_CTRL_COUNT      0x00010000  // Count Flag for STCTRL
#define ST_CTRL_CLK_SRC    0x00000004  // Clock Source for STCTRL
#define ST_CTRL_INTEN      0x00000002  // Interrupt Enable for STCTRL
#define ST_CTRL_ENABLE     0x00000001  // Enable for STCTRL

#define MAX_WAIT           0x27100

/* systick ISR layer function entry points */
extern void SysTick_initialization(void);
extern void clock_incr(void);

volatile int Got_time;
