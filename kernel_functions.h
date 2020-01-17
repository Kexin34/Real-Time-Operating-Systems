/**
 * kernel_functions.h
 * ECED 4402
 * Name: Kexin Wen
 * Date: 06 Nov 2018
 * Purpose: Include the name of kernel support function
 *          and structure of kernel message queue entry
 */

#ifndef KERNEL_FUNCTIONS_H_
#define KERNEL_FUNCTIONS_H_

#define MAX_MQ  255
#define MAX_ITEM 20
enum ISR_type {UART,TIME};
struct mqitem
{
    struct mqitem *next;
    int from;
    int size;
    char msg[255];
};

struct mqlist_entry
{
    struct mqitem *oldest;
    struct mqitem *youngest;
    int available;
    struct pcb *owner;
    int blocked;
    void *ptr_to_arglist;
};

struct mqitem MQData[MAX_ITEM];
extern char *str_ptr;
extern struct mqitem *freemqdata;
 struct mqlist_entry mqlist[MAX_MQ];

extern unsigned int K_terminate(void);
extern void k_GETID(struct kcallargs *kcaptr);
extern unsigned int K_bind(struct kcallargs *kcaptr);

//extern unsigned int k_send(unsigned to_mq, unsigned from_mq, void *msg, unsigned sz);
//extern unsigned int K_recv(unsigned to_mq, unsigned *from_mq, void *msg, unsigned sz,void *pmsgptr);

extern unsigned int k_send(unsigned  dst_mq, unsigned  src_mq, void *pmsg, unsigned size);
extern int K_recv (unsigned  dst_mq, unsigned  *src_mq, void *pmsg, unsigned  size);

extern int K_UART_STR(void *string, int len);
extern void K_uart_char(void *string);
#endif /* KERNEL_FUNCTIONS_H_ */
