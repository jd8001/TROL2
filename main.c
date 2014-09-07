#include "context.h"
int p1(void){
	unsigned char x = 0;
	x++;
	return 7;
}


	
			
int main (void){
	unsigned long int m=0x0F0F, *n;
	process *p;
	SYSTEM_INIT();
	declare_global("myglobal", &m, sizeof(unsigned long int));
	n = (unsigned long int*) get_global ("myglobal");
	m = 0xAAAA;
	m = *n;
	SET_RTOS_PERIOD(100000 USEC);
	p = new_process(p1, 1, 5, 0);
	while(1);
	return 0;

}

	
	
	
		
		
		
					
					
					
				
	
			
		
			
				
		