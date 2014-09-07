#include "dynamic.h"
volatile short  heap[HEAP_SIZE]; 
volatile unsigned char heap_map[HEAP_SIZE/8];
volatile unsigned char status;

void init_heap(void){
	unsigned int i;
	for (i=0; i<HEAP_SIZE; i++) heap[i] = 0xFFFF;
	for (i=0; i< HEAP_SIZE/8; i++) heap_map[i] = 0x0000;
	status = 0;
}	
void* malloc(unsigned short size){
	unsigned short sizec;		//word (16 bit) size of the data structure returned
	unsigned char mask = 0;
	unsigned int location;		//location in the heap
	unsigned int l;				//preserves heap location for reply

	/*generate the word size by dividing the byte size by two
	give the structure one extra byte if necessary to preserve
	word alignment */
	if (size & 0x01) sizec = 1 + (size >> 1);
	else sizec = size >> 1;
	/*find an available location inside the heap*/
	location = space_avail(sizec);
	l = location;
	
	if (status & MEMORY_OVERFLOW) return NULL;		//out of memory
	/*this if else takes care of the first byte of
	the map that needs to be marked */
	if (sizec + (location & 0x0007) > 7){
		
		heap_map[location >> 3] = 0xFF;
		sizec -= 7 - (location & 0x0007);
	}
	else{
		heap_map[location >> 3] = heap_map[location >> 3] << sizec;
		mask = 0xFF;
		mask = mask >> (8 - sizec);
		heap_map[location >> 3] |= mask;
		return &heap[l];
	}	
	//convert location to a heap map location
	//add one because the first byte was accounted
	//for above
	location = 1 + (location >> 3);	
	//mark in byte chunks	 
	while (sizec >=8){
		heap_map[location] = 0xFF;
		sizec -=8;
		location++;
	}
	
	//last byte
	mask = 0xFF >> (8 - sizec);
	heap_map[location] |=mask;
	return &heap[l];
}	

unsigned int space_avail(unsigned short size){
	unsigned char test, c;
	unsigned int loc, reply=0;
	unsigned short s=0;
		for (loc = 0; loc < (HEAP_SIZE >> 3); loc++){
			test = heap_map[loc];
			if (test == 0xFF){
				s = 0;
				reply = (8*(loc+1)) + 1;
			}
			else{	
				for (c = 0; c < 8; c++){
					if (! (test & 0x01)) s++;
					else {
						s = 0;
						reply = 1 + (8*loc) + (unsigned int) c;
					}
					if (s == size){
						return reply;
					}	
					test = test >> 1;
				}
			}
	
	
		}	
	status |= MEMORY_OVERFLOW;
	return 0;
}	
void free (void *x, unsigned short size){
	unsigned long int l = (unsigned long int) x - (unsigned long int) &heap[0];
	unsigned short sizec = (size/2);		
	unsigned char mask; 
	l = l >> 1;
	if (sizec > (8 - (l & 0x00000007))){		
		mask = 0xFF >> (8 - (l & 0x00000007));
		heap_map[l >> 3] &= mask;
		l = 1 + (l >> 3);
		sizec -= (8 - (l & 0x00000007));
	}
	else{
		mask = 0x01 << (l & 0x00000007); 
		while (sizec > 0){
			mask = mask | (mask << 1);
			sizec --;
		}
		heap_map[l >> 3] &= ~mask;
		return;
	}		
	while (sizec >=8){
		heap_map[l] = 0x00;
		sizec -=8;
		l++;
	}	
	mask = 0xFF << sizec;
	heap_map[l] &= mask;
	x = NULL;
}