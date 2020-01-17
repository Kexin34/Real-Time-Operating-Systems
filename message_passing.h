/**
 * message_passing.h
 * ECED 4402
 * Message passing layer module header file
 * Name: Kexin Wen
 * Date: 06 Nov 2018
 * Purpose: From Assignment#1, contains the structure of the queue
 */


#define QSIZE 64     /* Queue size */
#define TRUE    1
#define FALSE   0
#define NUL 0
#define UART_IN_Q 0
#define UART_OUT_Q 1

enum Queue_type {UART_IN,UART_OUT};            /* Two queue's name*/



/* From assignment #1, the original structure contains other fields */
struct UART_queue
{
    unsigned data;
};

/* Message passing layer function entry points */
//extern int recvmsg(char *data);
//extern void sendmsg(char data);
//extern void UART_Enqueue(enum Queue_type Q_name, char data);
//extern int UART_Dequeue(enum Queue_type Q_name, char *data);
extern int UART_Dequeue(enum Queue_type to, char *data);
extern void UART_Enqueue(enum Queue_type to, char data);
