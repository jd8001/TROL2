#ifndef DOUBLE_LINKED_LIST_H
#define DOUBLE_LINKED_LIST_H
#include "dynamic.h"
typedef struct{
	void* data;
	void* next;
	void* prev;
}list_node;
typedef struct{
	list_node *begin;
	list_node *end;
	unsigned long int size;
}list;
void push_back(list *l, void* d);
void insert(list *l, void *d, unsigned long int loc);
char remove(list *l, void *d);
void* get_data(list *l, unsigned long int loc);	
void build(list *x);
#endif