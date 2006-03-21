#ifndef queue_h
#define queue_h

void insque(void * entry, void * pred);
unsigned long remque(void * entry, void * addr);
void insqhi(void * entry, void * header);
unsigned long remqhi(void * header, void * addr);
void insqti(void * entry, void * header);
unsigned long remqti(void * header, void * addr);

int aqempty(void * q);
int rqempty(void * q);

void qhead_init(void * l);
void sickinsque(void * entry, void * pred);

INSQUE(long x, long y);
REMQUE(long * e, long * a);

#endif
