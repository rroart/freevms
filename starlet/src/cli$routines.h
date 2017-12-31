#ifndef cli$routines_h
#define cli$routines_h

unsigned int cli$dcl_parse(void * command_string, ...);

unsigned int cli$dispatch(int userarg, ...);

unsigned int cli$get_value(void *entity_desc, void *retdesc,...);

unsigned int cli$present(void *entity_desc);

#endif

