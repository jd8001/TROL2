#include "list.h"
void push_back(list *l, void *d){
	list_node *p = (list_node*)malloc(sizeof(list_node));
	p->data = d;
	l->size++;
	if (l->size == 1){
		l->begin = p;
		l->end = p;
		p->prev = NULL;
		p->next = NULL;
		return;
	}
	p->prev = l->end;
	l->end->next = p;
	p->next = NULL;
	l->end = p;
	return;
}	
void insert(list *l, void *d, unsigned long int loc){
	unsigned long int x;
	list_node *p = l->begin;
	list_node *q = (list_node*)malloc(sizeof(list_node));
	if (loc == 0){
		q->data = d;
		q->next = l->begin;
		q->prev = NULL;
		l->begin = q;
		l->size++;
		if (l->size == 1) l->end = q;
		return;
	}
	if (loc == l->size){
		push_back(l, d);
		return;
	}		
	for (x = 0; x < loc; x++) p = (list_node*)p->next;
	((list_node*)p->prev)->next = q;
	q->prev = p->prev;
	q->next = p;
	p->prev = q;
	l->size++;
	return;
}		


char remove(list *l, void *d){
	list_node *p;
	for (p=l->begin; p->data!=d && p!=l->end; p = (list_node*)p->next);
	if (p==NULL) return 0;
	l->size--;
	if (l->begin == p){
		l->begin = p->next;
		p->next = NULL;
		p->prev = NULL;
		return 1;
	}
	if (l->end == p){
		l->end = p->prev;
		p->next = NULL;
		p->prev = NULL;
		return 1;
	}	
	((list_node*)p->prev)->next = p->next;
	((list_node*)p->next)->prev = p->prev;
	p->next = NULL;
	p->prev = NULL;
	return 1;
}
void* get_data(list *l, unsigned long int loc){
	unsigned int num = loc;
	list_node *p = l->begin;
	while (num){
		p = p->next;
		num--;
	}
	return p->data;
}		
void build(list *x){
	x->begin = 0;
	x->end = 0;
	x->size = 0;
}	