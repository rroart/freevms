#ifndef queue_h
#define queue_h

void insque(void * entry, void * pred);
void remque(void * entry, void * addr);
void insqhi(void * entry, void * header);
void remqhi(void * header, void * addr);
void insqti(void * entry, void * header);
void remqti(void * header, void * addr);

#endif
