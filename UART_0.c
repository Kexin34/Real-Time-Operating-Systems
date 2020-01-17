/*
 * File: UART_0.c
 * Original Source: http://lh.ece.dal.ca/eced4402/Tiva_UART_EchoV1.c
 * Original Author: Emad Khan
 * Modified By: Stephen Sampson
 * Created on: Nov 12, 2018
 * Purpose: UART function definitions.
 */

#include "UART_0.h"
#include "message_passing.h"
#include "kernel_functions.h"
#include <string.h>
#include "initialization.h"
#include "KernelCalls.h"
void UART0_Init(void)
{
    volatile int wait;

    /* Initialize UART0 */
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCUART_GPIOA;   // Enable Clock Gating for UART0
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCGPIO_UART0;   // Enable Clock Gating for PORTA
    wait = 0; // give time for the clocks to activate

    UART0_CTL_R &= ~UART_CTL_UARTEN;        // Disable the UART
    wait = 0;   // wait required before accessing the UART config regs

    // Setup the BAUD rate
    UART0_IBRD_R = 8;   // IBRD = int(16,000,000 / (16 * 115,200)) = 8.680555555555556
    UART0_FBRD_R = 44;  // FBRD = int(.680555555555556 * 64 + 0.5) = 44.05555555555556

    UART0_LCRH_R = (UART_LCRH_WLEN_8);  // WLEN: 8, no parity, one stop bit, without FIFOs)

    GPIO_PORTA_AFSEL_R = 0x3;        // Enable Receive and Transmit on PA1-0
    GPIO_PORTA_PCTL_R = (0x01) | ((0x01) << 4);         // Enable UART RX/TX pins on PA1-0
    GPIO_PORTA_DEN_R = EN_DIG_PA0 | EN_DIG_PA1;        // Enable Digital I/O on PA1-0

    UART0_CTL_R = UART_CTL_UARTEN;        // Enable the UART
    wait = 0; // wait; give UART time to enable itself.
}

void InterruptEnable(unsigned long InterruptIndex)
{
/* Indicate to CPU which device is to interrupt */
if(InterruptIndex < 32)
    NVIC_EN0_R = 1 << InterruptIndex;       // Enable the interrupt in the EN0 Register
else
    NVIC_EN1_R = 1 << (InterruptIndex - 32);    // Enable the interrupt in the EN1 Register
}

void UART0_IntEnable(unsigned long flags)
{
    /* Set specified bits for interrupt */
    UART0_IM_R |= flags;
}
void InterruptMasterEnable(void)
{
    /* enable CPU interrupts */
    __asm(" cpsie   i");
}
void UART0_initialization()
{
    UART0_Init();           // Initialize UART0
    InterruptEnable(INT_VEC_UART0);       // Enable UART0 interrupts
    UART0_IntEnable(UART_INT_RX | UART_INT_TX); // Enable Receive and Transmit interrupts
    InterruptMasterEnable();      // Enable Master (CPU) Interrupts
}
void UART0_IntHandler(void)
{
/*
 * UART0 ISR - handles receive and xmit interrupts
 * Application signalled when data received
 */
    if (UART0_MIS_R & UART_INT_RX)
    {
        /* RECV done - clear interrupt */
        UART0_ICR_R |= UART_INT_RX;
        /* Set 'Data' to char received by UART */
        Data = UART0_DR_R;
        GotData = TRUE;
        //UART_Enqueue(UART_IN, Data);
        k_send(30,UART , &Data, 1) ;

        NVIC_INT_CTRL_R |= TRIGGER_PENDSV;
    }

    if (UART0_MIS_R & UART_INT_TX)
    {
        /* XMIT done - clear interrupt */
        UART0_ICR_R |= UART_INT_TX;
        int x;
        /* cause a short delay to fix clobbering*/
        for(x = 0; x < 1500;x++);
        /* if characters remain in UART_Q,dequeue next character to UART0_DR_R*/
        if((UART_Dequeue(UART_OUT,&TX_Data)))
            UART0_DR_R = TX_Data;
    }
 }

void char_echo(char data)
{
    char ch[2];
    ch[0] = data;
    ch[1] = NULL;
    uart_output_str(ch);
    //K_UART_STR(ch, 1);
}



