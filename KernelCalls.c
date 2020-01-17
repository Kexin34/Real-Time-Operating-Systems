/**
 * KernelCalls.c
 * Class: ECED 4402
 * Assignment#2
 * Author Name: Kexin Wen
 * Date: 01 Nov 2018
 * Purpose: Sample kernel call using SVC
 *          Use of stack to pass arguments to kernel
 */
#include <stdio.h>
#include "KernelCalls.h"
#include "process_management.h"


int getid()
/* Return the current PID stored in PCB */
{
    return pkcall(GETID, NULL);
}

void terminate()
{
    pkcall(TERMINATE, NULL);
}

int nice(unsigned new_priority)
/* Kernel allow a process to change its priority */
{
    volatile struct kcallargs args;
    args.code = NICE;
    args.saddr = new_priority;
    assignR7((unsigned long) &args);
    SVC();
    /* return value indicate operation succefull or fail */
    return args . rtnvalue;
}

int bind(volatile unsigned queue_no)
{
    volatile struct kcallargs args;
    args.code = BIND;
    args.saddr = queue_no;
    assignR7((unsigned long) &args);
    SVC();
    return args . rtnvalue;
}

int p_send_msg(int to, int from, void *msg, unsigned size)
{
    /*
    *@param to: the message queue for the process need to be updated
    *@param from: the process that send the message
    *@param msg: address of the message
    *@param size: the number of bytes in the message
    *@return: result of send operation (Successful or Failure)
    This is interruptible. It put the arguments into a structure that can be read by the kernel and then call the kernel.
    */
    struct p_msg_struct pmsg; //  message structure
    pmsg . to_mq = to;
    pmsg . from_mq = from;
    pmsg . msg = msg;
    pmsg . sz = size;
    return pkcall(SEND_MESSAGE, &pmsg); /* pkcall() return code ignored */
}

int p_recv_msg(unsigned to_mq, unsigned *from_mq,  void *msg, unsigned size)
{
    /* Describe: Return the message when it arrives.
     * @param to_mq: the queue from which it expects to receive a message
     * @param from_mq: indicate the sender PID
     * @param msg: the address of the message structure
     * @param size: the maximum number of bytes that can be copied from k-space to p-space.
     * @return: The size of the returned message(must be less-than-or-equal to the maximum size
     *         specified in the argument size; a negative value can indicate an error)
     * @return: sender's queue number
     */
    struct p_msg_struct pmsg;
    pmsg . to_mq = to_mq;
    pmsg . msg = msg;
    pmsg . sz = size;
    pkcall(RECV_MESSAGE, &pmsg);
    /* A message will be available at this point */
    /*while(pmsg.rtncode == FAILURE)
    {
        pkcall(RECV_MESSAGE, &pmsg);
    }*/
    //*from_mq= pmsg->from_mq;
     return pmsg.sz;
}

void uart_output_str(void *string)
{
    struct usrt_string uart_str;
    uart_str.data = string;
    uart_str.len = strlen(string);
     pkcall(UART_OUT_STR, &uart_str);
}

int uart_output_ch(int row, int col, char ch)
{
    /* Output a single character to specified screen position */
    /* CUP (Cursor position) command plus character to display */
    /* Assumes row and col are valid (1 through 24 and 1 through 80, respectively) */
    struct CUPch uart_data;
    /* Since globals aren’t permitted, this must be done each call */
    uart_data . esc = '\033';
    uart_data . sqrbrkt = '[';
    uart_data . line[0] = '0' + row / 10;
    uart_data . line[1] = '0' + row % 10;
    uart_data . semicolon = ';';
    uart_data . col[0] = '0' + col / 10;
    uart_data . col[1] = '0' + col % 10;
    uart_data . cmdchar = 'H';
    uart_data . nul = ch;
    return pkcall(UART_OUT_CH, &uart_data);
}

int pkcall(int code, void *pkmsg)
{
    /*
    Process-kernel call function.  Supplies code and kernel message to the
    kernel is a kernel request.  Returns the result (return code) to the caller.
    @param code: the name of the kernel action to be performed
    @param pkmsg: address of the structure containing the data regarding the message
    */
    volatile struct kcallargs arglist;
    /* Pass code and pkmsg to kernel in arglist structure */
    arglist . code = code;
    arglist . saddr = (void*)pkmsg; //point to structure we want to pass to kernel
    /* R7 = address of arglist structure */
    assignR7((unsigned long) &arglist);
    /* Call kernel */
    SVC();
    /* Return result of request to caller */
    return arglist . rtnvalue;
}

void assignR7(volatile unsigned long data)
{
    /* Assign 'data' to R7; since the first argument is R0, this
     * is simply a MOV from R0 to R7;
     */
    __asm("     mov r7, r0");
}

