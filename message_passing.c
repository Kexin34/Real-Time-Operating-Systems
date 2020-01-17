/**
 * message_passing.c
 * Class: ECED 4402
 * Assignment#2
 * Author Name: Kexin Wen
 * Date: 01 Nov 2018
 * Purpose: From Assignment#1,the UART_queues is circular in nature with a fixed size.
 *          Contain the enqueue and dequeue function of UART_queues.
 */
#include<stdio.h>
#include<stdlib.h>
#include "message_passing.h"

/*initialize monitor & UART_OUT queue */
struct UART_queue queue[2][QSIZE];
int uart_in_head = 0;
int uart_in_tail = QSIZE-1;
int uart_in_tx;
int uart_out_head = 0;
int uart_out_tail = QSIZE-1;
int uart_out_tx;


void UART_Enqueue(enum Queue_type to, char data)
{
    /* Enqueue UART char/Tick signal from ISR to monitor Queue */
    if(to == UART_IN){
        /* If monitor queue is not FULL, Enqueue the sourceID and data, hardware input could only be UART/SYSTICK */
        if(uart_in_head != uart_in_tail){
            queue[UART_IN_Q][uart_in_head].data = data;
            uart_in_head = (uart_in_head + 1) % QSIZE;
        }
    }
    /* Enqueue char from monitor to UART_OUT Queue */
    else{
        /* If UART_OUT queue is not FULL, Enqueue the string's char into UART_OUT queue  */
        if(uart_out_head != uart_out_tail){
            queue[UART_OUT_Q][uart_out_head].data = data;
            uart_out_head = (uart_out_head + 1) % QSIZE;
        }
    }
}


int UART_Dequeue(enum Queue_type to, char *data)
{
    if(to == UART_IN){                              /* Dequeue char from UART Queue */
        uart_in_tx = (uart_in_tail + 1) % QSIZE;
        if(uart_in_tx == uart_in_head)              /* empty monitor queue*/
            return FALSE;
        else{                                       /* Otherwise, PULL the entry from the tail */
            *data = queue[UART_IN_Q][uart_in_tx].data;
            uart_in_tail = uart_in_tx;
            return TRUE;
        }
    }else{                                          /* Dequeue from UART_OUT Queue for output */
        uart_out_tx = (uart_out_tail + 1) % QSIZE;
        if(uart_out_tx == uart_out_head)            /* Empty UART_OUT queue*/
            return FALSE;                           /* Otherwise, PULL the entry from the tail */
        else{
            *data = queue[UART_OUT_Q][uart_out_tx].data;
            uart_out_tail = uart_out_tx;
            return TRUE;
        }
    }
}



