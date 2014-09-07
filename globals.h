#ifndef GLOBALS_H
#define GLOBALS_H
#include "dynamic.h"
#include "context.h"
typedef struct{
	char *name;
	void *data;
	unsigned int size;
	unsigned long int access_time;
}globe;	

int init_globals(void);
int declare_global(char *n, void *d, unsigned int s);
void free_global(char *n);
void* get_global(char *n);
char compare_string(char *a, char *b);
unsigned short size (char *a);
#endif

