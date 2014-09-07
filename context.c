#include "context.h"
volatile list process_list, context_list;
volatile unsigned int CURRENT_TICK;
volatile unsigned short context_temp;
volatile process *CURRENT_PROCESS;
volatile unsigned short wc0, wc1, wc2, wc3, wc4, wc5, wc6, wc7, wc8, wc9, wc10;
volatile unsigned short wc11, wc12, wc13, wc14, wc15, rcc;
void __attribute__((interrupt(preprologue("mov w15, _wc15\n mov w14, _wc14\n mov w13, _wc13\n mov w12, _wc12\n mov w11, _wc11\n mov w10, _wc10\n mov w9, _wc9\n mov w8, _wc8\n mov w7, _wc7\n mov w6, _wc6\n mov w5, _wc5\n mov w4, _wc4\n mov w3, _wc3\n mov w2, _wc2\n mov w1, _wc1\n mov w0, _wc0\n mov RCOUNT, WREG \n mov WREG, _rcc")), shadow)) _T1Interrupt(void){
	IFS0 &= 0xFFF7;
	SR &= 0xFF1F;
	TROLL_MAIN();
}
int SYSTEM_IDLE(void){
	unsigned int x=0;
	unsigned int y=5;
	while(1);
	
}
int SET_RTOS_PERIOD(unsigned long int t){
	//argument is in clock ticks
	//sets the interrupt period (PR1)
	//to the number of clock ticks specified
	//true for success false for failure
	
	unsigned long int clock_cycles = t, old;
	unsigned short mask, PROLD = PR1, i, preold, pre;
	
	/*disable the interrupt while playing with
	the clock*/
	IEC0 &= 0xFFF7;
	
	/*compute the old prescaler*/
	preold = (T1CON & 0x0030) >> 4;
	if (preold == 0) preold = 1;
	if (preold == 1) preold = 8;
	if (preold == 2) preold = 64;
	if (preold == 3) preold = 256;
	
	old = PROLD*preold;			//gives the old number of clock cycles per period


	if (clock_cycles <= 65535){
		pre = 1;
		mask = 0x0000;					//1:1 prescaler
	}
	if (clock_cycles > 65535){
		pre = 8;
		mask = 0x0010;				//8 prescaler
	}	
	if (clock_cycles > 524280){
		pre = 64;
		mask = 0x0020;				//64 prescaler
	}	
	if (clock_cycles > 4194240){
		pre = 256;
		mask = 0x0030;				//256 prescaler
	}
	if (clock_cycles > 16776960){
		//too large
		IEC0 |= 0x0008;
		return 0;
	}	

	

	
	
	PR1 = t/pre;
	T1CON &= 0xFFCF;					//clear out the precaler bits
	T1CON |= mask;						//insert new prescaler

	
	/*adjust all the periods in the process list to match
	up to the new RTOS_PERIOD*/
	for (i= 0; i<process_list.size; i++)
		((process*)get_data(&process_list, i))->period*=(double)((double)old/(double)t);

	TMR1 = 0;
	IEC0 |=0x0008;
	return 1;
}	
	
	
unsigned long int msec2RT(unsigned long int t){
	return usec2RT(t*1000);
}	
unsigned long int usec2RT(unsigned long int t){
	unsigned long int clock_cycles;
	unsigned short pow, i;
	unsigned short pre;
	
	clock_cycles = (FOSC/(2e6))*t;		//this is the number of clock cycles
										//needed to generate t microseconds

	pow = (T1CON &0x0030) >> 4  ;		//8^pow is the prescaler value on T1
	
	if (!pow) return clock_cycles;
		
	if (pow == 1) pre = 8;
	if (pow == 2) pre = 64;
	if (pow == 3) pre = 256;
	
	clock_cycles/=pre;
	
	return clock_cycles/PR1;			//returns the number of interrupts
										//or "RTOS_TICKS" for the number
										//of usec specified
}
void SYSTEM_INIT(void){
	process *idle_process;
/*clock and interrupt*/
	T1CON = 0x8030;
	OSCCON = 0x0060;
	CLKDIV = 0x0000;
	PLLFBD = 0x0030;
	IPC0 &= 0x0FFF;
	IPC0 |= 0x7000;
	IFS0 &= 0xFFF7;
	IEC0  |= 0x0008;
	PR1 = 0x00FF;	
	SR &= 0xFF1F;
	
/*push the idle process onto the context list*/
	build(&context_list);
	build(&process_list);
	idle_process = new_process(SYSTEM_IDLE, 255, 1, 0);
	insert(&context_list, idle_process, 0);
	CURRENT_PROCESS->status = 0;
	

/*variable init*/
	CURRENT_TICK = 0;
	init_globals();	
}
		
void TROLL_MAIN(void){
	void *tmp;
	void (*x)(void) = NULL;
	process *p, *q;
	unsigned char done = 0, sw=0;
	unsigned short i,j;
	x = &SWITCH_CONTEXT;
	CURRENT_TICK++;
	/*look for process who need to run*/
	for (i=0; i<process_list.size; i++){
		p = (process*)get_data(&process_list, i);
		if ((p->execution_tick == CURRENT_TICK) && !(p->status & PROCESS_RUNNING)){
			q = (process*)get_data(&context_list, 0);
			j = 0;
			while ((q->priority < p->priority) && (j < context_list.size)){
				j++;
				q = (process*)get_data(&context_list, j);
			}
			if (j==0) sw = 1;		//signifies a switch needs to occur
			insert(&context_list, p, j);
		}
	}
	if (sw) goto *x	;	//switch context
}
void SWITCH_CONTEXT(void){
		process *q;
		unsigned long int *r;
		q = (process*)get_data(&context_list, 0);
		//preserve old context
		if (CURRENT_PROCESS->status & PROCESS_RUNNING){
		CURRENT_PROCESS->wreg_context[0] = wc0;
		CURRENT_PROCESS->wreg_context[1] = wc1;
		CURRENT_PROCESS->wreg_context[2] = wc2;
		CURRENT_PROCESS->wreg_context[3] = wc3;
		CURRENT_PROCESS->wreg_context[4] = wc4;
		CURRENT_PROCESS->wreg_context[5] = wc5;
		CURRENT_PROCESS->wreg_context[6] = wc6;
		CURRENT_PROCESS->wreg_context[7] = wc7;
		CURRENT_PROCESS->wreg_context[8] = wc8;
		CURRENT_PROCESS->wreg_context[9] = wc9;
		CURRENT_PROCESS->wreg_context[10] = wc10;
		CURRENT_PROCESS->wreg_context[11] = wc11;
		CURRENT_PROCESS->wreg_context[12] = wc12;
		CURRENT_PROCESS->wreg_context[13] = wc13;
		CURRENT_PROCESS->wreg_context[14] = wc14;
		CURRENT_PROCESS->wreg_context[15] = wc15;
		}			
		//r = top of the stack
		r = q->wreg_context[15];
		*r = q->last_PC;
		q->wreg_context[15]+=4;
		q->status |= PROCESS_RUNNING;
		CURRENT_PROCESS = q;
		WREG0 = q->wreg_context[0];
		WREG1 = q->wreg_context[1];
		WREG2 = q->wreg_context[2];
		WREG3 = q->wreg_context[3];
		WREG4 = q->wreg_context[4];
		WREG5 = q->wreg_context[5];
		WREG6 = q->wreg_context[6];
		WREG7 = q->wreg_context[7];
		WREG8 = q->wreg_context[8];
		WREG9 = q->wreg_context[9];
		WREG10 = q->wreg_context[10];
		WREG11 = q->wreg_context[11];
		WREG12 = q->wreg_context[12];
		WREG13 = q->wreg_context[13];
		context_temp = q->wreg_context[15];
		SPLIM = (unsigned long int)q->stack + q->stack_size;
		WREG14 = q->wreg_context[14];
		WREG15 = context_temp;
		__asm__ ("RETURN");
		

			
		
	}
void EXECUTION_CONTEXT_SWITCHER(void){
	unsigned short i;
	unsigned long int *ecs;
	CURRENT_PROCESS->execution_tick += CURRENT_PROCESS->period;
	CURRENT_PROCESS->status &= ~PROCESS_RUNNING;
	/*reset the context*/
	for (i=0; i<14; i++) CURRENT_PROCESS->wreg_context[i]=0;
	CURRENT_PROCESS->last_PC = CURRENT_PROCESS->entry_point;
	CURRENT_PROCESS->wreg_context[14] = CURRENT_PROCESS->stack;
	ecs = CURRENT_PROCESS->wreg_context[14]+2;
	*ecs = &EXECUTION_CONTEXT_SWITCHER;
	CURRENT_PROCESS->wreg_context[15] = CURRENT_PROCESS->wreg_context[14]+6;
	remove(&context_list, get_data(&context_list, 0));
	SWITCH_CONTEXT();
}	
	
process* new_process(int (*callback)(void), unsigned char pr, unsigned long int pe, unsigned int st){
	//builds a new process
	//places it in the process list
	process *reply = (process*)malloc(sizeof(process));
	void (*x)(void) = &EXECUTION_CONTEXT_SWITCHER;
	unsigned long int ecs = x;
	unsigned long int *a;
	/*build the stack
	if st is 0 then the default stack size is used
	otherwise st represents the user defined stack size*/
	if (st == 0){
		reply->stack = malloc(DEFAULT_STACK_SIZE);
		reply->stack_size = DEFAULT_STACK_SIZE;
	}	
	else{
		reply->stack = malloc(st);			
		reply->stack_size = st;
	}
	/*set various parameters*/
	reply->priority = pr;
	reply->period = pe;
	reply->execution_tick = CURRENT_TICK + pe;
	/*this code initializes the stack so that when the process
	finishes it will return to EXECUTION_CONTEXT_SWITCHER*/
/*	*((unsigned long int*)reply->sp) = ecs;
	reply->sp+=2;*/
	
	//set the entry point
	reply->entry_point = callback;
	reply->last_PC = callback;
	
	//set W14 and W15 as stack and frame pointers
	reply->wreg_context[14] = (unsigned short) reply->stack;
	a = reply->wreg_context[14]+2;
	*a = ecs;
	reply->wreg_context[15] = reply->wreg_context[14]+6;
	
	//push this process onto the process list
	push_back(&process_list, reply);
	return reply;
}	