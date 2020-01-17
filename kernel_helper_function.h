/**
 * kernel_helper_functions.h
 * Class: ECED 4402
 * Assignment#2
 * Author Name: Kexin Wen
 * Date: 03 Nov 2018
 * Purpose: Includes name of helper functions called by the kernel
 *          support routine in kernel_function.c module.
 */

#ifndef KERNEL_HELPER_FUNCTION_H_
#define KERNEL_HELPER_FUNCTION_H_
#define BLOCKED_Q   5

extern void remove_current_process(void);
extern void updating_new_prio_list(unsigned new_priority);
extern void dequeue(struct pcb *pcbptr, int de_priority);
extern void enqueue(struct pcb *pcbptr, int en_priority);
extern void k_send_blocked(unsigned to_mq, unsigned from_mq, void *msg, unsigned sz);
extern void k_send_unblocked(unsigned to_mq, unsigned from_mq, void *msg, unsigned sz);
extern int k_recv_no_msg(unsigned to_mq,void *pmsgptr);
extern int k_recv_from_kernel_queue(unsigned to_mq, unsigned *from_mq, void *msg, unsigned sz, void *pmsgptr);
extern void mqfree(struct mqitem *mqp);
#endif /* KERNEL_HELPER_FUNCTION_H_ */
