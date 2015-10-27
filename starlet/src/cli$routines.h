#ifndef CLI_ROUTINES_H
#define CLI_ROUTINES_H

int cli$dcl_parse(void * command_string, ...);

int cli$dispatch(int userarg, ...);

int cli$get_value(void *entity_desc, void *retdesc,...);

int cli$present(void *entity_desc);

#endif /* CLI_ROUTINES_H */
