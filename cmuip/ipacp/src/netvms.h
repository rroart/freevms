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

#define DESC$STR_ALLOC(x,y) /* check */ \
char x##_str[y];\
struct dsc$descriptor x##_act={dsc$w_length:y,dsc$a_pointer:x##_str}, *x = &x##_act

#endif
