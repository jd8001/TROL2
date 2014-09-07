#ifndef DYNAMIC_H
#define DYNAMIC_H
#define HEAP_SIZE 2500
#define MEMORY_OVERFLOW 0x01
#define NULL 0
void init_heap(void);
void* malloc(unsigned short size);
unsigned int space_avail(unsigned short size);
void free (void *x, unsigned short size);
#endif
