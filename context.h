#ifndef CONTEXT_H
#define CONTEXT_H
#include "dynamic.h"
#include "list.h"
#include "globals.h"
#include "p24hj128gp502.h"
#define DEFAULT_STACK_SIZE 256
#define PROCESS_RUNNING 0x01
#define FOSC 80e6
#define USEC *FOSC/(2*1e6)
#define MSEC *FOSC/(2*1e3)
typedef struct{
	void *stack;	//points to the process' stack
	unsigned char priority;
	unsigned long int execution_tick;
	unsigned long int period;
	unsigned short wreg_context[16];
	void *last_PC;
	unsigned char status;
	int (*entry_point)(void);	//beginning of the process
	unsigned short SR;
	unsigned int stack_size;
	unsigned long int sp;
}process;
process* new_process(int (*callback)(void), unsigned char pr, unsigned long int pe, unsigned int st);
void TROLL_MAIN(void);	
void SWITCH_CONTEXT(void);
void SYSTEM_INIT(void);
int SYSTEM_IDLE(void);
unsigned long int usec2RT(unsigned long int t);
unsigned long int msec2RT(unsigned long int t);
int SET_RTOS_PERIOD(unsigned long int t);
#endif