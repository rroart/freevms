#ifndef netvms_h
#define netvms_h

#define FALSE 0
#define TRUE 1
#define OFF 0
#define ON 1

struct queue_header_structure {
struct queue_header_structure * si_qhead;
struct queue_header_structure * si_qtail;
};

#endif
