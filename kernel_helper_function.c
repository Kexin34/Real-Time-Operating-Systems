/**
 * kernel_helper_functions.c
 * Class: ECED 4402
 * Assignment#2
 * Author Name: Kexin Wen
 * Date: 03 Nov 2018
 * Purpose: Includes definitions of functions called by the kernel support routine
 *          in kernel_function.c module.
 */
#include "kernel_functions.h"
#include "kernel_helper_function.h"
#include "KernelCalls.h"
#include "initialization.h"
#include "process_management.h"

#include <string.h>

struct mqitem *mq_malloc()
{
    struct mqitem *mqp;
    mqp = freemqdata;
    freemqdata = freemqdata->next;
    return mqp;
}

void mqfree(struct mqitem *mqp)
{
    mqp -> next= freemqdata;
    freemqdata = mqp;
}

void remove_current_process(void)
{
    /* remove process from current priority list */
    running[priority] -> prev-> next = running[priority] -> next;
    running[priority] -> next-> prev = running[priority] -> prev;
}
void updating_new_prio_list(unsigned new_priority)
{
    if(running[new_priority]== NULL){
         running[new_priority] = running[priority];
         running[new_priority]-> next = running[priority];
         running[new_priority]-> prev = running[priority];
    }else
    {
        running[priority] -> next = running[new_priority]->next;
        running[new_priority]->next -> prev= running[priority];
        running[new_priority]->next = running[priority] ;
        running[priority]-> prev =running[new_priority] ;
        //running[new_priority] = running[priority];
    }
}

void k_send_blocked(unsigned to_mq, unsigned from_mq, void *msg, unsigned sz)
{
    /* Copy the message to received process*/
    /* Copy bytes from message to receiver */
    //struct p_msg_struct *aptr = mqlist[to_mq].ptr_to_arglist;
    struct pcb *pcbptr = mqlist[to_mq].owner;
    //int check_id =pcbptr->id;

    //char *pptr = (char*)aptr->msg;    /* recv message pointer */
    //char *kptr = (char*)msg;    /* send message pointer */
    //unsigned maxsz;
    //maxsz = (aptr->sz > sz)?sz: aptr->sz ;
    //int ind;
    //for( ind = 0; ind < maxsz; ind++)
    //    *pptr++ = *kptr++;
    /*Dequeue from BLCOCKED Q */
    dequeue(pcbptr, BLOCKED_Q);
    /* Enqueue to running queue */
    enqueue(pcbptr, pcbptr->priority);
    mqlist[to_mq].blocked = FALSE;
    return SUCCESS;
}

void k_send_unblocked(unsigned to_mq, unsigned from_mq, void *msg, unsigned sz)
{
    /*
     * Copy the sender message to kernel message queue
     * The copied size is depend on sender message size
     */
    struct mqitem *mdptr = mq_malloc();
    int count;
    char *mpptr = (char*)msg;       /* p-space pointer, point to sender message */
    char *mkptr;                    /* K-space pointer, point to message in kernel queue message */
    mdptr->from = from_mq;
    mdptr->size = sz;
    mkptr = (char*)mdptr -> msg;
    for(count = 0; count < sz; count++)
        *mkptr++ = *mpptr++;

    /* Add the copied message item into youngest position in its queue */
    if(mqlist[to_mq].youngest != NULL)
    {
        mqlist[to_mq].youngest-> next = mdptr;
        mqlist[to_mq].youngest= mdptr;
        mdptr -> next = NULL;
    }else
    {
        mqlist[to_mq].youngest= mdptr;
        mqlist[to_mq].oldest = mdptr;
        mdptr -> next = NULL;
    }
}
int k_recv_no_msg(unsigned to_mq,void *pmsgptr)
{
    struct pcb *pcb_ptr = mqlist[to_mq].owner;
    save_registers();
    running[priority] -> sp = get_PSP();
    //running[priority] = pcb_ptr -> next;

    mqlist[to_mq].blocked = TRUE;
    pcb_ptr->state = BLOCKED;

    mqlist[to_mq].ptr_to_arglist = pmsgptr;

    struct p_msg_struct *aptr = mqlist[to_mq].ptr_to_arglist;
    int ss =aptr->sz;
    /* Dequeue receiver pcb from running queue */
    dequeue(pcb_ptr, pcb_ptr->priority);

    /* Enqueue into Block Queue */
    enqueue(pcb_ptr,BLOCKED_Q);

    /* update PCB and PSP to allow next process to run */
    while(running[priority]== NULL)
            priority--;

    set_PSP(running[priority] -> sp);
    int next_id = running[priority] ->id;
    return SUCCESS;
}


void dequeue(struct pcb *pcbptr, int de_priority)
{
    if(running[de_priority]->next == running[de_priority]){
        running[de_priority] = NULL;
    } else {
        pcbptr->prev->next = pcbptr->next;
        pcbptr->next->prev = pcbptr->prev;
        running[de_priority] = pcbptr -> next;
    }
}

void enqueue(struct pcb *pcbptr, int en_priority)
{
    if (running[en_priority] == NULL) {
        pcbptr->next = pcbptr;
        pcbptr->prev = pcbptr;
        running[en_priority] = pcbptr;
    } else {
        /*pcbptr->next = running[en_priority];
        pcbptr->prev = running[en_priority]->prev;
        running[en_priority]->prev->next = pcbptr;
        running[en_priority]->prev = running[en_priority]->prev->next;*/

        pcbptr->prev = running[en_priority]->prev;
        running[en_priority]->prev->next = pcbptr;
        pcbptr->next = running[en_priority];
        running[en_priority]->prev = pcbptr;
    }
}
int k_recv_from_kernel_queue(unsigned to_mq, unsigned *from_mq, void *msg, unsigned sz, void *pmsgptr )
{
    /* send the oldest message in kernel queue to p-space*/
    struct mqitem *mqdptr;

    mqdptr = mqlist[to_mq].oldest;
    *from_mq = mqdptr->from;
    char *pptr = (char*)msg;                        /* recv message pointer*/
    char *kptr = (char*)mqdptr->msg;                /* pointer point to message field in message queue*/
    int copysize = (mqdptr->size > sz) ? sz:mqdptr->size ;
    int ind;
    for( ind = 0; ind < copysize; ind++)
            *pptr++ = *kptr++;
    /* De-link the copied message entry in queue and free the space */
    mqfree(mqdptr);
    return copysize;
}
