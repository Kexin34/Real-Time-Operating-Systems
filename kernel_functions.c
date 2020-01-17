/**
 * kernel_functions.c
 * Class: ECED 4402
 * Assignment#2
 * Author Name: Kexin Wen
 * Date: 01 Nov 2018
 * Purpose: Includes definitions of functions called by the kernel.
 */
#include "KernelCalls.h"
#include "process_management.h"
#include "kernel_functions.h"
#include "kernel_helper_function.h"
#include "systick.h"
#include "UART_0.h"
#include "message_passing.h"
#include <string.h>

struct mqlist_entry mqlist[MAX_MQ];
struct mqitem *freemqdata;
char *str_ptr;

void Kernel_Initialization(void)
{
    /* Ensure PendSV priority is set to lowest (7) */
    NVIC_SYS_PRI3_R |= PENDSV_LOWEST_PRIORITY;

    /* Set up message queue */
    int mq_ind;
    for(mq_ind = 0; mq_ind < MAX_MQ; mq_ind++ )
        mqlist[mq_ind] = (struct mqlist_entry){ .available = TRUE, .blocked = FALSE, .oldest = NULL, .youngest = NULL };

    /* Create free message queue data */
    int rr;
    freemqdata = (struct mqitem *)malloc(sizeof(struct mqitem));
    for(rr = 0; rr < MAX_ITEM; rr++)
    {
        struct mqitem *new_item;
        new_item = (struct mqitem *)malloc(sizeof(struct mqitem));
        new_item->next = freemqdata;
        freemqdata = new_item;
    }
}

void k_GETID(struct kcallargs *kcaptr)
{
    kcaptr -> rtnvalue = running[priority] -> id;
}

unsigned int K_terminate()
{
    /* if current process is the last process in current priority */
    if(running[priority]->next == running[priority])
    {
        free(running[priority]->sp);
        free(running[priority]);
        running[priority]= NULL;
    }
    else
    {
        /* Break link */
        running[priority]->prev->next = running[priority] ->next;
        running[priority] ->next->prev = running[priority] ->prev;

        /* Deallocate stack */
        free(running[priority]->sp);//?? original stk addr??
        struct pcb *temp;
        temp = running[priority];
        running[priority] = temp-> next;

        /* Deallocate PCB */
        free(temp);
    }
    /* For those no process left in current priority, run the lower non-null entry */
    while(running[priority]== NULL)
        priority--;
    /*return New PSP */
    set_PSP(running[priority] -> sp);
    return running[priority] -> sp;
}

unsigned int K_bind(struct kcallargs *kcaptr)
{
    /* Kernel create a queue associated with this queue number and associate the process with it.
     * If a queue already exist and is associate another process, it return -1 indicate fail.
     */
    int bind_no = kcaptr->saddr;
    if (mqlist[bind_no].available)
    {
        /* bind is succefull */
        mqlist[bind_no].available = FALSE;
        mqlist[bind_no].owner = running[priority];
        return SUCCESS;
    }
    else
        return FAILURE;
}

/* Send specified message to specified message queue */
unsigned int k_send_wen(unsigned to_mq, unsigned from_mq, void *msg, unsigned sz)
{
    /*
     * update to_mq with address of msg, from_mq, and size
     * return: indication send succeesful or failure
     */

    /* Check 'TO' */
    if (mqlist[to_mq].available)
        /* The destination process doesn't bind to a message queue */
        return SEND_NO_MQ;

    /* Check 'FROM' */
    if (mqlist[from_mq].owner != running[priority])
        return FAILURE;

    if (mqlist[to_mq].blocked)
        /* message is availble to receiver, copy message to receiver */
        k_send_blocked(to_mq,from_mq, msg, sz);
    else
        /* Received process running, store the message in kernel destination message queue */
        k_send_unblocked(to_mq,from_mq, msg, sz);
    return SEND_DONE;
}


unsigned int k_send(unsigned  dst_mq, unsigned  src_mq, void *pmsg, unsigned size) {
    /*Update the message queue for the process associated with ‘dst_mq’ with the address of ‘pmsg’
     * and the sender’s message queue, ‘src_mq’. The number of bytes
     * in the message is specified in ‘size’. This is uninterruptible, called within k-space only.
     */

    if(mqlist[dst_mq].available == TRUE) //check from
        return FAILURE;
    if((mqlist[src_mq].owner != running[priority])&&(src_mq != UART)&&(src_mq != TIME)) //check to see if owner equals to running
        return FAILURE;
    struct mqitem *tmp =  mq_malloc();  // (struct mqData *)malloc(sizeof(struct mqData)); //TBD
    tmp->from = src_mq;
    tmp->size = size;
    /*char *mpptr = (char*)pmsg;       /* p-space pointer, point to sender message
    char *mkptr = (char*)tmp ->msg;                    /* K-space pointer, point to message in kernel queue message
    int count;
    for(count = 0; count < size; count++)
        *mkptr++ = *mpptr++;*/
    int i ;
    char rr;
    for(i  = 0; i < size; i++) //copy message
    {
        rr = ((char *)pmsg)[i];
        tmp->msg[i] = rr;
    }
    tmp->msg[i] = '\0';
    tmp->next = NULL;

    if(mqlist[dst_mq].oldest == NULL) //check if the oldest block is available or not
    {
        mqlist[dst_mq].oldest = tmp;
        mqlist[dst_mq].youngest = tmp;
        tmp -> next = NULL;
    }
    else // if oldest is already being used
    {
        mqlist[dst_mq].youngest->next = tmp;
        mqlist[dst_mq].youngest = tmp;
        tmp -> next = NULL;
    }

    if(mqlist[dst_mq].blocked == TRUE)
    { //if it is blocked
        struct pcb *tmpPcb = mqlist[dst_mq].owner;

        char *pptr = (char*)mqlist[dst_mq].ptr_to_arglist;
        char *kptr = (char*)pmsg;
        //unsigned maxsz= (aptr->sz > size)?size: aptr->sz ;
        int ind;
        for( ind = 0; ind < size; ind++)
            *pptr++ = *kptr++;
        /*struct p_msg_struct *aptr = mqlist[dst_mq].ptr_to_arglist;//
        char *pptr = (char*)aptr->msg;    //recv message pointer
        char *kptr = (char*)pmsg;    // send message pointer
        unsigned maxsz;
        int ii = aptr->sz;
        maxsz = (aptr->sz > size)?size: aptr->sz ;
        int ind;
        for( ind = 0; ind < maxsz; ind++)
            *pptr++ = *kptr++;
         */

        if(running[tmpPcb->priority] == NULL)
        {
            tmpPcb->next = tmpPcb;
            tmpPcb->prev = tmpPcb;
            running[tmpPcb->priority] = tmpPcb;
        }
        else
        {
            tmpPcb->prev = running[tmpPcb->priority]->prev;
            running[tmpPcb->priority]->prev->next = tmpPcb;
            tmpPcb->next = running[tmpPcb->priority];
            running[tmpPcb->priority]->prev = tmpPcb;
        }
    }


    return SUCCESS;
}


int K_recv (unsigned  dst_mq, unsigned  *src_mq, void *pmsg, unsigned  size) {
    /*Update the message queue for the process associated with ‘dst_mq’ with the address of ‘pmsg’
     * and the sender’s message queue, ‘src_mq’. The number of bytes
     * in the message is specified in ‘size’. This is uninterruptible, called within k-space only.
     */
    int i = 0;
    if(mqlist[dst_mq].oldest == NULL)
    //no message, blocked itself
    {
        if(mqlist[dst_mq].blocked == FALSE){
            running[priority] -> sp = get_PSP();

            mqlist[dst_mq].ptr_to_arglist = pmsg;

            //if it is the only process in current priority
            if(running[priority] == running[priority]->next) {
                running[priority] = NULL;
                priority = 5;
                while(running[priority] == NULL) {
                    priority -= 1;
                }
            }else{
                //dequeue from running queue
                running[priority]->prev->next = running[priority]->next;
                running[priority]->next->prev = running[priority]->prev;
                running[priority] = running[priority]->next;
            }
            mqlist[dst_mq].blocked = TRUE;
            set_PSP(running[priority] -> sp);
        }
        return FAILURE;
    }
    else
    {
        if(mqlist[dst_mq].blocked == TRUE)
        {
            struct pcb *tmpPcb = mqlist[dst_mq].owner;
            if(running[tmpPcb->priority] == NULL)
            {
                tmpPcb->next = tmpPcb;
                tmpPcb->prev = tmpPcb;
                running[tmpPcb->priority] = tmpPcb;
            }
            else
            {
                tmpPcb->prev = running[tmpPcb->priority]->prev;
                running[tmpPcb->priority]->prev->next = tmpPcb;
                tmpPcb->next = running[tmpPcb->priority];
                running[tmpPcb->priority]->prev = tmpPcb;
            }
            mqlist[dst_mq].blocked = FALSE;
        }
        struct mqitem *dataList = mqlist[dst_mq].oldest;
        mqlist[dst_mq].oldest = mqlist[dst_mq].oldest->next;
        *src_mq = dataList->from;

        int sizeOfCopy = (size < dataList->size) ? size : dataList->size;
        char *targetMsg = (char *)(dataList->msg);
        char *srcMsg = (char *)(pmsg);
        for(i = 0; i < sizeOfCopy; i++)
        {
            srcMsg[i] = targetMsg[i];
        }
        mqfree(dataList);
        return SUCCESS;
    }
}




/* Receive message from specified message queue if it exists. Block process if not. */
unsigned int K_recv_wen(unsigned to_mq, unsigned *from_mq, void *msg, unsigned sz,void *pmsgptr)
{
    /* check validity of the destination message queue is current process */
    if(mqlist[to_mq].owner != running[priority])
        return FAILURE;                             /* ERROR */

    /* If its own message queue contain message */
    if(mqlist[to_mq].oldest != NULL)
        /*copy message from kernel queue to location specified by receiver */
        return k_recv_from_kernel_queue(to_mq, &from_mq,(char *)msg,sz, pmsgptr);
    else
        /* message queue is empty, block the current process until message arrive on queue */
        return k_recv_no_msg(to_mq, pmsgptr);
}

/* Function description:Kernel support routine for UART transmit */
int K_UART_STR(void *string, int len)
{
    char *data_ptr = (char *)string;
    int str_idx;
    for(str_idx = 0;str_idx < len; str_idx++)
        UART_Enqueue(UART_OUT,data_ptr[str_idx]);
    UART0_DR_R ='\0';
    return 1;
}

/* Function description: Kernel support routine for changing current process priority */
void K_NICE(unsigned new_priority)
{
    remove_current_process();
    /* check is new priority is higher/lower than current priority */
    if(new_priority > priority){
        /* promote itself to higher priority */
        updating_new_prio_list(new_priority);
        priority = new_priority;
    }
    else if(new_priority < priority){
        /* "demote" itself to lower priority and stop running */
        updating_new_prio_list(new_priority);
    }
}

