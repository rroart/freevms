#ifndef queue_h
#define queue_h

void insque(void * entry, void * pred);
unsigned long remque(void * entry, void * addr);
void insqhi(void * entry, void * header);
unsigned long remqhi(void * header, void * addr);
void insqti(void * entry, void * header);
unsigned long remqti(void * header, void * addr);

#endif
