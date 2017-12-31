#ifndef netvms_h
#define netvms_h

#define FALSE 0
#define TRUE 1
#define OFF 0
#define ON 1

// a few ascii characters.

#define     CH_NUL	  0
#define     CH_tab	  9
#define     CH_sp	  32
#define     CH_FF	  12		// formFeed
#define     CH_LF	  10		// Line-Feed
#define     CH_CR	  13		// Carriage-Return

struct queue_header_fields
{
    struct queue_header_fields * qhead;
    struct queue_header_fields * qtail;
};

#define queue_header_structure(x) x

#define DESC$STR_ALLOC(x,y) /* check */ \
char x##_str[y];\
struct dsc$descriptor x##_act={dsc$w_length:y,dsc$a_pointer:x##_str}, *x = &x##_act

#define ASCID(x) ((struct dsc$descriptor){dsc$w_length:strlen(x)-1,dsc$a_pointer:x})
#define ASCID2(y,x) ((struct dsc$descriptor){dsc$w_length:y,dsc$a_pointer:x})
#define ASCIDNOT(x) x

#endif
