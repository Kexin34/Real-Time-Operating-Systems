/**
 * KernelCalls.h
 * ECED 4402
 * Name: Kexin Wen
 * Date: 01 Nov 2018
 * Purpose: Contains the structure of kernel call argument, based on the type of service
 *          Set kernel-required functions from process-space
 */
#ifndef KERNELCALLS_H_
#define KERNELCALLS_H_

/* PendSV handler access */
#define NVIC_INT_CTRL_R (*((volatile unsigned long *) 0xE000ED04))
#define TRIGGER_PENDSV 0x10000000
/* PendSV handler is operating at lowest priority 7 */
#define NVIC_SYS_PRI3_R (*((volatile unsigned long *) 0xE000ED20))
#define PENDSV_LOWEST_PRIORITY 0x00E00000

#define BADCODE -1
#define SUCCESS 1
#define FAILURE 0
#define TRUE    1
#define FALSE   0
#define NULL    0
enum kernelcallcodes {GETID, NICE, TERMINATE, SEND_MESSAGE, RECV_MESSAGE, BIND, UART_OUT_CH,UART_OUT_STR,UART_RX};
enum process_state   {BLOCKED, RUNNING, WAIT};
enum message_operation {SEND_NO_MQ, SEND_DONE};

struct kcallargs
{
    unsigned int code;      /* Unique (and defined) kernel code */
    unsigned int rtnvalue;  /* Result of operation (specific to each code) */
    unsigned long saddr;    /* Pointer to structure to pass to kernel, Address (32-bit value) of process message */

};

struct p_msg_struct
{
    int to_mq;     /* Destination queue */
    int from_mq;     /* Source queue */
    void *msg;      /* Pointer to message to be supplied to the receiver */
    int sz;         /* Number of bytes in the message (pointed to by msg) */
    int rtncode;   /* Result of send() */
};

struct usrt_string
{
    void *data;
    int len;
};
/*
struct p_msg_time
{
    unsigned short sqid;    // Src
    void *msg;  // Msg
    unsigned short sz;  // Size
    unsigned int rtncode;
    struct CUPch *vt100;
}; */
//int p_req_time(unsigned short sqId, char *inTime, unsigned short size, unsigned short row);

struct CUPch
{
    char esc;
    char sqrbrkt;
    char line[2];   /* 01 through 24 */
    char semicolon;
    char col[2];    /* 01 through 80 */
    char cmdchar;
    char nul;
};

extern void assignR7(volatile unsigned long data);
extern void terminate();
extern int getid();
extern int nice(unsigned new_priority);
extern int pkcall(int code, void *pkmsg);
extern void Kernel_Initialization(void);
extern int bind(volatile unsigned queue_no);
extern int p_send_msg(int to, int from, void *msg, unsigned size);
extern int p_recv_msg(unsigned to_mq, unsigned *from_mq,  void *msg, unsigned size);
extern int uart_output_ch(int row, int col, char ch);

extern void uart_output_str(void* string);

#endif /*KERNELCALLS_H_*/
