/**
 * initialization.h
 * Class: ECED 4402
 * Assignment#2
 * Author Name: Kexin Wen
 * Date: 06 Nov 2018
 * Contain all the process functions, structure of certain application structure (time)
 */
#ifndef INITIALIZATION_H_
#define INITIALIZATION_H_

#define NVIC_INT_CTRL_R (*((volatile unsigned long *) 0xE000ED04))
#define TRIGGER_PENDSV 0x10000000

struct time_info
{
    int sec;
    int min;
    int hour;
};


struct time_info current_time;

extern long priority;
extern void startup(void);
extern char* itoa(int num, char* str, int base);
extern void time_initialization(void);
extern const char * current_time_provide(void);

extern void idle_proc(void);
/* Time server application */

extern void time_request(void);
extern void func13(void);

/* UART application: printing CUPch */
extern void UART_application_1(void);
extern void UART_application_2(void);
extern void UART_application_3(void);
extern void UART_application_4(void);

/* Case converter */
extern void lower_upper_case(void);
extern void output_converted_case(void);

/*  Testing messaging */
extern void func11(void);
extern void func12(void);


extern void func21(void);
extern void func22(void);

extern void receiver(void);
extern void receiver2(void);
extern void sender(void);

/* REAL case convert */
extern void monitor(void);
extern void case_convert_output(void);
/* REAL Time server application */
extern void time_server(void);
extern void time_request(void);

/* For testing case */
extern void Proc5(void);
extern void Proc4(void);
extern void Proc3_1(void);
extern void Proc3_2(void);
extern void Proc2(void);

void build_string(char*cmd_line_buffer, char data,int *index);
#endif /* INITIALIZATION_H_ */
