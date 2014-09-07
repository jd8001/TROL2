#include "globals.h"
extern volatile unsigned int CURRENT_TICK;
volatile list GLOBAL_VARIABLE_LIST;

int init_globals(void){
	build (&GLOBAL_VARIABLE_LIST);
	return 1;
}
int declare_global(char *n, void *d, unsigned int s){
	globe *g = (globe*)malloc(sizeof(globe));
	unsigned short i=0, temp;
	char *x= n, *y;
	void *p;
	
	//figure out how long the name is
	while (*x != 0){
		i++; x++;
	}	
	i++;
	x = (char*)malloc(i);
	y = x;
	while (*n!=0){
		*x = *n;
		x++; n++;
	}
	g->name = y;
	g->data = malloc(s);
	p = g->data;
	i = s;
	while (i > 0){
		*((short*)(g->data)) = *((int*)d);
		g->data+=2; d+=2; i--;
	}	 
	g->data = p;
	g->size = s;
	g->access_time = 0;
	push_back(&GLOBAL_VARIABLE_LIST, g);
	return 1;
}	
void free_global(char *n){
	unsigned short i = 0;
	globe *c;
	while (i < GLOBAL_VARIABLE_LIST.size){
		i++;
		c = (globe*)get_data(&GLOBAL_VARIABLE_LIST, i);
		if (compare_string(n, c->name)){
			free(c->name, size(c->name));
			free(c->data, c->size);
			return;
		}	
	}
	return;
}
void* get_global(char *n){
	globe *c;
	int i=0;
	c = (globe*)get_data(&GLOBAL_VARIABLE_LIST, i);
	while (!compare_string(c->name, n)){ 		
		i++;
		c = (globe*)get_data(&GLOBAL_VARIABLE_LIST, i);
		if (i == GLOBAL_VARIABLE_LIST.size) return 0;
	}
	c->access_time = CURRENT_TICK;
	return c->data;
}

char compare_string(char *a, char *b){
	char *c=a, *d= b;
	while ((*c == *d) && *c!=0 && *d!=0){
		c++; d++;
	}
	if ((*c==0) && (*d==0)) return 1;
	return 0;
}
unsigned short size (char *a){
	char *b = a;
	unsigned short reply=0;
	while (b!=0) {
		reply++; b++;
	}
	return reply;
}				
			
	
	 			