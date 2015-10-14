#ifndef TIMETEST_H
#define TIMETEST_H

#include <gen64def.h>

int call_sys$asctim(unsigned short *time_name_len, char *time_name, struct _generic_64 *date_time);

int call_sys$bintim(char *time_name, struct _generic_64 *date_time);

int call_sys$gettim(struct _generic_64 *date_time);

int call_sys$numtim(unsigned short int *time_vector, struct _generic_64 *date_time);

void run_time_test(void);

#endif /* TIMETEST_H */
